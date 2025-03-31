#include "include/SystemMonitorWindow.hpp"
#include "CpuUsage.hpp"
#include "FileSystem.hpp"
#include "NetInfo.hpp"
#include "NvidiaInfo.hpp"
#include "glibmm/error.h"
#include "glibmm/main.h"
#include "glibmm/refptr.h"
#include "gtk/gtk.h"
#include "gtkmm/csssection.h"
#include "gtkmm/enums.h"
#include "gtkmm/object.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/stylecontext.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <gtkmm/cssprovider.h>
#include <cstddef>
#include <glibmm.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/statvfs.h>
#include <thread>
#include <chrono>
#include <utility>

SystemMonitorWindow::SystemMonitorWindow()
    : m_VBox(Gtk::Orientation::VERTICAL, 5),
      m_HBox(Gtk::Orientation::HORIZONTAL, 5), m_frame_cpu(cpu.get_cpu_name()),
      m_frame_ram("RAM"), m_frame_gpu(nvidia.get_nvidia_gpu_name()), m_frame_fs("Disks"),
      m_box_cpu(Gtk::Orientation::VERTICAL, 5),
      m_box_gpu(Gtk::Orientation::VERTICAL, 5),
      m_box_ram(Gtk::Orientation::VERTICAL, 5),
      m_ram_gpu_box(Gtk::Orientation::HORIZONTAL)
{
  // get ip data
  ipData = netInfo.get_ip_address();

  // set title for the window
  set_title("System Monitor");
  // set window size
  set_default_size(800, 800);

  add_css_class("main-window");
  
  m_HBox.set_margin(5);
  set_child(m_HBox);
  m_HBox.append(m_VBox);
  
  prev = cpu.get_cpu_tread_data();
  size_t cpu_count = prev.size();

  m_grid_cpu.set_column_homogeneous(true);
  m_grid_cpu.set_column_spacing(6);
  m_grid_cpu.set_row_spacing(10);

  for (size_t i = 0; i < cpu_count; i++)
  {
    auto prog_bar = Gtk::make_managed<Gtk::ProgressBar>();
    m_progressbar_cpu.push_back(prog_bar);
    
    prog_bar->set_margin(5);
    prog_bar->set_halign(Gtk::Align::CENTER);
    prog_bar->set_valign(Gtk::Align::CENTER);
    // prog_bar->set_size_request(100, -1);
    prog_bar->set_text("CPU" + std::to_string(i));
    prog_bar->set_show_text(true);prog_bar->set_hexpand();
    prog_bar->set_vexpand();
    prog_bar->set_valign(Gtk::Align::CENTER);


    int col = i % 3;
    int row = i / 3;
    m_grid_cpu.attach(*prog_bar, col, row);
  }

  m_frame_cpu.set_child(m_grid_cpu);
  m_frame_cpu.set_hexpand(true);
  m_frame_cpu.set_valign(Gtk::Align::FILL);
  m_frame_cpu.add_css_class("cpu-frame");
  
  m_dispatcher.connect([this]()
  {
    for (size_t i = 0; i < cpuUsageData.size() && i < m_progressbar_cpu.size(); i++)
    {
      double usageFraction = cpuUsageData[i] / 100.0;
      m_progressbar_cpu[i]->set_fraction(usageFraction);
      m_progressbar_cpu[i]->add_css_class("cpu-progress-bar");
      // m_progressbar_cpu[i]->set_size_request(300, 25);
      // m_progressbar_cpu[i]->set_vexpand(true);
      // m_progressbar_cpu[i]->set_hexpand(true);
      // m_progressbar_cpu[i]->set_hexpand(25);
      // m_progressbar_cpu[i]->set_vexpand(25);
      // m_progressbar_cpu[i]->set_valign(Gtk::Align::CENTER);
      
      if (i == 0)
      {
        m_progressbar_cpu[0]->set_text("Total CPU " + two_decimals_format(cpuUsageData[0]) + "%");
      }
      else
      {
        m_progressbar_cpu[i]->set_text("CPU" + std::to_string(i) + " " + two_decimals_format(cpuUsageData[i]) + "%");
      }
    }
  });

  // allow cpu frame to expand
  m_frame_cpu.set_hexpand(true);
  m_frame_cpu.set_valign(Gtk::Align::FILL);
  // set cpu css class
  m_frame_cpu.add_css_class("cpu-frame");
  
  // update cpu progress bar
  Glib::signal_timeout().connect([this]() { return update_cpu_progress_bar(); }, 1000);

  update_cpu_progress_bar();
  
  m_frame_ram.set_child(m_box_ram);
  m_frame_ram.add_css_class("ram-frame");
  m_box_ram.append(m_progressbar_mem_tot);
  m_progressbar_mem_tot.set_margin(5);
  m_progressbar_mem_tot.set_halign(Gtk::Align::CENTER);
  m_progressbar_mem_tot.set_valign(Gtk::Align::CENTER);
  m_progressbar_mem_tot.set_show_text(true);
  m_progressbar_mem_tot.set_size_request(300, -1);
  m_progressbar_mem_tot.set_vexpand(true);
  m_progressbar_mem_tot.set_hexpand(true);

  m_box_ram.append(m_progressbar_mem_used);
  m_progressbar_mem_used.set_margin(5);
  m_progressbar_mem_used.set_halign(Gtk::Align::CENTER);
  m_progressbar_mem_used.set_valign(Gtk::Align::CENTER);
  m_progressbar_mem_used.set_size_request(300, -1);
  m_progressbar_mem_used.set_show_text(true);
  m_progressbar_mem_used.set_vexpand(true);
  m_progressbar_mem_used.set_hexpand(true);
  
  m_box_ram.append(m_progressbar_mem_available);
  m_progressbar_mem_available.set_margin(5);
  m_progressbar_mem_available.set_halign(Gtk::Align::CENTER);
  m_progressbar_mem_available.set_valign(Gtk::Align::CENTER);
  m_progressbar_mem_available.set_show_text(true);
  m_progressbar_mem_available.set_size_request(300, -1);
  m_progressbar_mem_available.set_vexpand(true);
  m_progressbar_mem_available.set_hexpand(true);

  m_box_ram.append(m_progressbar_mem_free);
  m_progressbar_mem_free.set_margin(5);
  m_progressbar_mem_free.set_halign(Gtk::Align::CENTER);
  m_progressbar_mem_free.set_valign(Gtk::Align::CENTER);
  m_progressbar_mem_free.set_show_text(true);
  m_progressbar_mem_free.set_size_request(300, -1);
  m_progressbar_mem_free.set_vexpand(true);
  m_progressbar_mem_free.set_hexpand(true);

  m_progressbar_mem_tot.add_css_class("mem-progressbar-tot");
  m_progressbar_mem_used.add_css_class("mem-progressbar-used");
  m_progressbar_mem_available.add_css_class("mem-progressbar-available");
  m_progressbar_mem_free.add_css_class("mem-progressbar-free");
  
  Glib::signal_timeout().connect([this]() { return update_mem_usage(); }, 1000);

  update_mem_usage();

  m_frame_gpu.set_child(m_box_gpu);
  m_frame_gpu.add_css_class("gpu-frame");
  m_box_gpu.append(m_progressbar_gpu_nvidia_gpuUtil);
  m_progressbar_gpu_nvidia_gpuUtil.set_margin(5);
  m_progressbar_gpu_nvidia_gpuUtil.set_halign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_gpuUtil.set_valign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_gpuUtil.set_show_text(true);
  m_progressbar_gpu_nvidia_gpuUtil.set_size_request(300, -1);
  m_progressbar_gpu_nvidia_gpuUtil.set_vexpand(true);
  m_progressbar_gpu_nvidia_gpuUtil.set_hexpand(true);

  m_box_gpu.append(m_progressbar_gpu_nvidia_memUtil);
  m_progressbar_gpu_nvidia_memUtil.set_margin(5);
  m_progressbar_gpu_nvidia_memUtil.set_halign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_memUtil.set_valign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_memUtil.set_show_text(true);
  m_progressbar_gpu_nvidia_memUtil.set_size_request(300, -1);
  m_progressbar_gpu_nvidia_memUtil.set_vexpand(true);
  m_progressbar_gpu_nvidia_memUtil.set_hexpand(true);
  
  m_box_gpu.append(m_progressbar_gpu_nvidia_usedVram);
  m_progressbar_gpu_nvidia_usedVram.set_margin(5);
  m_progressbar_gpu_nvidia_usedVram.set_halign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_usedVram.set_valign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_usedVram.set_show_text(true);
  m_progressbar_gpu_nvidia_usedVram.set_size_request(300, -1);
  m_progressbar_gpu_nvidia_usedVram.set_vexpand(true);
  m_progressbar_gpu_nvidia_usedVram.set_hexpand(true);

  m_box_gpu.append(m_progressbar_gpu_nvidia_freeVram);
  m_progressbar_gpu_nvidia_freeVram.set_margin(5);
  m_progressbar_gpu_nvidia_freeVram.set_halign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_freeVram.set_valign(Gtk::Align::CENTER);
  m_progressbar_gpu_nvidia_freeVram.set_show_text(true);
  m_progressbar_gpu_nvidia_freeVram.set_size_request(300, -1);
  m_progressbar_gpu_nvidia_freeVram.set_vexpand(true);
  m_progressbar_gpu_nvidia_freeVram.set_hexpand(true);

  m_progressbar_gpu_nvidia_gpuUtil.add_css_class("gpu-progressbar-nvidia-gpuUtil");
  m_progressbar_gpu_nvidia_memUtil.add_css_class("gpu-progressbar-nvidia-memUtil");
  m_progressbar_gpu_nvidia_usedVram.add_css_class("gpu-progressbar-nvidia-usedVram");
  m_progressbar_gpu_nvidia_freeVram.add_css_class("gpu-progressbar-nvidia-freeVram");

  Glib::signal_timeout().connect([this]() { return update_nvidia_gpu_usage(); }, 1000);

  update_nvidia_gpu_usage();

  m_frame_net.set_child(m_box_net);
  m_frame_net.set_label(ipData.name + " " + ipData.addr);
  m_box_net.append(m_progressbar_net_rx);
  m_progressbar_net_rx.set_margin(5);
  m_progressbar_net_rx.set_halign(Gtk::Align::CENTER);
  m_progressbar_net_rx.set_valign(Gtk::Align::CENTER);
  m_progressbar_net_rx.set_size_request(300, -1);
  m_progressbar_net_rx.set_show_text(true);
  m_progressbar_net_rx.set_vexpand(true);
  m_progressbar_net_rx.set_hexpand(true);
  m_progressbar_net_rx.add_css_class("net-progressbar-rx");

  m_box_net.append(m_progressbar_net_tx);
  m_progressbar_net_tx.set_margin(5);
  m_progressbar_net_tx.set_halign(Gtk::Align::CENTER);
  m_progressbar_net_tx.set_valign(Gtk::Align::CENTER);
  m_progressbar_net_tx.set_size_request(300, -1);
  m_progressbar_net_tx.set_show_text(true);
  m_progressbar_net_tx.set_vexpand(true);
  m_progressbar_net_tx.set_hexpand(true);
  m_progressbar_net_tx.add_css_class("net-progressbar-tx");

  Glib::signal_timeout().connect([this]() { return  update_net_usage(); }, 1000);

  m_frame_fs.set_child(m_box_fs);

  auto mnts = filesSystem.get_mounted_file_system();
  for (const auto &mnt : mnts)
  {
    if (mnt.device.find("/dev") == 0 && mnt.fsType != "tmpfs" && mnt.fsType != "proc" && mnt.fsType != "sysfs")
    {
      auto* bar = Gtk::make_managed<Gtk::ProgressBar>();
      bar->set_margin(5);
      bar->set_show_text(true);
      bar->set_halign(Gtk::Align::CENTER);
      bar->set_valign(Gtk::Align::CENTER);
      bar->set_size_request(300, -1);
      m_box_fs.append(*bar);
      m_progressbar_fs.push_back({mnt.mountPoint, bar});
    }
  }    
  
  
  Glib::signal_timeout().connect([this]() {
    for (const auto& disk : m_progressbar_fs)
    {
      update_disk_usage(disk.mntPoint);
    }
    return true;
  }, 1000);
  
  m_VBox.append(m_frame_cpu);
  m_ram_gpu_box.set_spacing(5);
  m_ram_gpu_box.append(m_frame_ram);
  m_ram_gpu_box.append(m_frame_gpu);
  m_VBox.append(m_ram_gpu_box);
  m_VBox.append(m_frame_net);
  m_VBox.append(m_frame_fs);

  m_ref_css_provider = Gtk::CssProvider::create();
#if HAS_STYLE_PROVIDER_ADD_PROVIDER_FOR_DISPLAY
  Gtk::StyleProvider::add_provider_for_display(get_display(), m_ref_css_provider,
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#else
  Gtk::StyleContext::add_provider_for_display(get_display(), m_ref_css_provider,
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#endif

  m_ref_css_provider->signal_parsing_error().connect(
      [](const auto &section, const auto &err)
        { on_parsing_error(section, err); });
  m_ref_css_provider->load_from_path("../css/style.css");
}

SystemMonitorWindow::~SystemMonitorWindow() {}

bool SystemMonitorWindow::update_cpu_progress_bar()
{
  std::thread([this]()
  {
    prev = cpu.get_cpu_tread_data();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    curr = cpu.get_cpu_tread_data();
    std::vector<CpuPercentage> cpuUsage = cpu.calculate_cpu_thread_usage(curr, prev);

    cpuUsageData.clear();
    for (const auto& usage : cpuUsage)
    {
      cpuUsageData.push_back(usage.percentageUsed);
    }

    m_dispatcher.emit();

  }).detach();

  return true;
}

void SystemMonitorWindow::on_parsing_error(const Glib::RefPtr<const Gtk::CssSection>& section, const Glib::Error & error)
{
  std::cerr << "on_parsing_error(): " << error.what() << "\n";
  if (section)
  {
    const auto file = section->get_file();
    if (file)
    {
      std::cerr << "  URI = " << file->get_uri() << "\n";
    }

    auto start_location = section->get_start_location();
    auto end_location = section->get_end_location();

    std::cerr << "  start_line = " << start_location.get_lines()+1
    << ", end_line = " << end_location.get_lines()+1 << "\n";

    std::cerr << "  start_position = " << start_location.get_line_chars()
    << ", end_position = " << end_location.get_line_chars() << "\n";
  }
}

bool SystemMonitorWindow::update_mem_usage()
{
  MemData data = memInfo.get_mem_data();

  // TODO display less decimales
  
  // convert from kb to gb
  double totGIB = data.memTotal / 1024.0 / 1024.0;
  double freeGIB = data.memFree / 1024.0 / 1024.0;
  double availableGIB = data.memAvailable / 1024.0 / 1024.0;
  double usedGIB = totGIB - availableGIB;

  // calculate percentage
  double percentageUsed = usedGIB / totGIB;
  double percentageAvailable = availableGIB / totGIB;
  double percentageFree = freeGIB / totGIB;

  // float x = std::floor(totGIB * 100.0) / 100.0f;
  
  m_progressbar_mem_tot.set_text("Total memory: " + two_decimals_format(totGIB) + " GIB");
  m_progressbar_mem_used.set_text("Memory used: " + two_decimals_format(usedGIB) + " GIB");
  m_progressbar_mem_available.set_text("Available memory: " + two_decimals_format(availableGIB) + " GIB");
  m_progressbar_mem_free.set_text("Free memory: " + two_decimals_format(freeGIB) + " GIB");
  
  m_progressbar_mem_tot.set_fraction(1.0);
  m_progressbar_mem_used.set_fraction(percentageUsed);
  m_progressbar_mem_available.set_fraction(percentageAvailable);
  m_progressbar_mem_free.set_fraction(percentageFree);

  return true;
}

bool SystemMonitorWindow::update_nvidia_gpu_usage()
{
  NvidiaData data = nvidia.calculateNvml();

  m_progressbar_gpu_nvidia_gpuUtil.set_text("GPU util: " + two_decimals_format(data.gpuUtil) + " %");
  m_progressbar_gpu_nvidia_memUtil.set_text("MEM util: " + two_decimals_format(data.memUtil) + " %");
  // m_progressbar_gpu_nvidia_totVram.set_text("Total Vram: " + std::to_string(data.totVram) + " %");

  m_progressbar_gpu_nvidia_gpuUtil.set_fraction(data.gpuUtil / 100);
  m_progressbar_gpu_nvidia_memUtil.set_fraction(data.memUtil / 100);
  // m_progressbar_gpu_nvidia_totVram.set_fraction(data.totVram / 100);

  // avoid division by 0
  if (data.totVram > 0)
  {
    double usedFraction = data.usedVram / data.totVram;
    double freeFraction = (data.totVram - data.usedVram) / data.totVram;
    m_progressbar_gpu_nvidia_usedVram.set_fraction(usedFraction);
    m_progressbar_gpu_nvidia_freeVram.set_fraction(freeFraction);

    m_progressbar_gpu_nvidia_usedVram.set_text("Used Vram: " + two_decimals_format(data.usedVram) + " GIB" + " / " + two_decimals_format(data.totVram) + " GIB");
    m_progressbar_gpu_nvidia_freeVram.set_text("Free Vram: " + two_decimals_format(data.totVram - data.usedVram) + " GIB" + " / " + two_decimals_format(data.totVram) + " GIB");
  }
  else
  {
    m_progressbar_gpu_nvidia_usedVram.set_fraction(0.0);
    m_progressbar_gpu_nvidia_freeVram.set_fraction(0.0);
  }
  
  return true;
}

std::string SystemMonitorWindow::two_decimals_format(double value)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << value;
  return ss.str();
}

bool SystemMonitorWindow::update_net_usage()
{
    static auto prev = netInfo.get_network_stats();
    auto curr = netInfo.get_network_stats();

    for (const auto& curr_stat : curr)
    {
      // for (const auto& prev_stat : prev)
      // {
      if (curr_stat.iface == "lo") continue;

      auto prev_stat = std::find_if(prev.begin(), prev.end(),
                                  [&curr_stat](const auto& stat) { return stat.iface == curr_stat.iface; });

      if (prev_stat != prev.end())
      {
          
          uint64_t rx_diff = curr_stat.rx_bytes - prev_stat->rx_bytes;
          uint64_t tx_diff = curr_stat.tx_bytes - prev_stat->tx_bytes;

          double rx_kb = rx_diff / 1024.0;
          double tx_kb = tx_diff / 1024.0;

          constexpr double maxKbPerSec = 125.0 * 1024.0;

          m_progressbar_net_rx.set_text("↓ " + two_decimals_format(rx_kb) + " KB/s");
          m_progressbar_net_rx.set_fraction(std::min(rx_kb / maxKbPerSec, 1.0));
          m_progressbar_net_tx.set_text("↑ " + two_decimals_format(tx_kb) + " KB/s");
          m_progressbar_net_tx.set_fraction(std::min(tx_kb / maxKbPerSec, 1.0));
      }
    }
    prev = std::move(curr);
  return true;
}

bool SystemMonitorWindow::update_disk_usage(const std::string& path)
{
  struct statvfs stat;

  if (statvfs(path.c_str(), &stat))
  {
    perror(("statvfs error for: " + path).c_str());
    return true;
  }

  uint64_t tot = stat.f_blocks * stat.f_frsize;
  uint64_t free = stat.f_bavail * stat.f_frsize;
  uint64_t used = tot - free;

  double usage = (double)used / (double)tot;

  for (auto &disk : m_progressbar_fs)
  {
    if (disk.mntPoint == path)
    {
      disk.bar->set_fraction(usage);
      disk.bar->set_text(path + ": " + two_decimals_format(usage * 100.0) + " % used");
      disk.bar->add_css_class("disk-progress-bar");
      break;
    }
  }    
  
  return true;
}
