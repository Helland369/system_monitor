#include "include/SystemMonitorWindow.hpp"
#include "CpuUsage.hpp"
#include "FileSystem.hpp"
#include "NetInfo.hpp"
#include "gdk/gdkkeysyms.h"
#include "gtk/gtk.h"
#include "gdkmm/enums.h"
#include "gtkmm/csssection.h"
#include "gtkmm/enums.h"
#include "gtkmm/eventcontrollerkey.h"
#include "gtkmm/object.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/stylecontext.h"
#include <gtkmm/cssprovider.h>
#include "glibmm/error.h"
#include "glibmm/main.h"
#include "glibmm/refptr.h"
#include <glibmm.h>
#include "glib.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstddef>
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
  : VBox_(Gtk::Orientation::VERTICAL, 5)
  , HBox_(Gtk::Orientation::HORIZONTAL, 5)
  , frame_cpu_("CPU")
  , frame_ram_("mem")
  , frame_fs_("disks")
  , box_cpu_(Gtk::Orientation::VERTICAL, 5)
  , box_gpu_(Gtk::Orientation::VERTICAL, 5)
  , box_ram_(Gtk::Orientation::VERTICAL, 5)
  , ram_fs_box_(Gtk::Orientation::HORIZONTAL, 5)
  , box_fs_(Gtk::Orientation::VERTICAL, 5)
  , box_net_(Gtk::Orientation::VERTICAL, 5)
{
  // get ip data
  ip_data_ = net_info_.get_ip_address();

  // set title for the window
  set_title("System Monitor");
  // set window size
  set_default_size(800, 800);

  add_css_class("main-window");

  HBox_.set_margin(5);
  set_child(HBox_);
  HBox_.append(VBox_);

  prev_ = cpu_.get_cpu_tread_data();
  size_t cpu_count = prev_.size();

  grid_cpu_.set_column_homogeneous(true);
  grid_cpu_.set_column_spacing(6);
  grid_cpu_.set_row_spacing(10);

  for (size_t i = 0; i < cpu_count; i++)
  {
    auto prog_bar = Gtk::make_managed<Gtk::ProgressBar>();
    progressbar_cpu_.push_back(prog_bar);

    prog_bar->set_margin(5);
    prog_bar->set_halign(Gtk::Align::CENTER);
    prog_bar->set_valign(Gtk::Align::CENTER);
    prog_bar->set_size_request(300, 50);
    prog_bar->set_text("CPU" + std::to_string(i));
    prog_bar->set_show_text(true);
    prog_bar->set_hexpand(true);
    prog_bar->set_vexpand(true);
    prog_bar->set_valign(Gtk::Align::FILL);

    int col = i % 3;
    int row = i / 3;
    grid_cpu_.attach(*prog_bar, col, row);
  }

  frame_cpu_.set_child(grid_cpu_);
  frame_cpu_.set_hexpand(true);
  frame_cpu_.set_valign(Gtk::Align::FILL);
  frame_cpu_.add_css_class("cpu-frame");
  frame_cpu_.set_label(cpu_.get_cpu_name());

  m_dispatcher.connect([this]()
                       {
    for (size_t i = 0; i < cpu_usage_data_.size() && i < progressbar_cpu_.size(); i++)
    {
      double usageFraction = cpu_usage_data_[i] / 100.0;
      progressbar_cpu_[i]->set_fraction(usageFraction);
      progressbar_cpu_[i]->add_css_class("cpu-progress-bar");

      if (i == 0)
      {
        progressbar_cpu_[0]->set_text("Total CPU " + two_decimals_format(cpu_usage_data_[0]) + "%");
      }
      else
      {
        progressbar_cpu_[i]->set_text("CPU " + std::to_string(i) + " " + two_decimals_format(cpu_usage_data_[i]) + "%");
      }
    } });

  // allow cpu frame to expand
  frame_cpu_.set_hexpand(true);
  frame_cpu_.set_valign(Gtk::Align::FILL);
  // set cpu css class
  frame_cpu_.add_css_class("cpu-frame");

  // update cpu progress bar
  Glib::signal_timeout().connect([this]()
                                 { return update_cpu_progress_bar(); },
                                 1000);

  update_cpu_progress_bar();

  frame_ram_.set_child(box_ram_);
  frame_ram_.add_css_class("ram-frame");
  box_ram_.append(progressbar_mem_tot_);
  progressbar_mem_tot_.set_margin(5);
  progressbar_mem_tot_.set_halign(Gtk::Align::CENTER);
  progressbar_mem_tot_.set_valign(Gtk::Align::CENTER);
  progressbar_mem_tot_.set_show_text(true);
  progressbar_mem_tot_.set_size_request(300, -1);
  progressbar_mem_tot_.set_vexpand(true);
  progressbar_mem_tot_.set_hexpand(true);

  box_ram_.append(progressbar_mem_used_);
  progressbar_mem_used_.set_margin(5);
  progressbar_mem_used_.set_halign(Gtk::Align::CENTER);
  progressbar_mem_used_.set_valign(Gtk::Align::CENTER);
  progressbar_mem_used_.set_size_request(300, -1);
  progressbar_mem_used_.set_show_text(true);
  progressbar_mem_used_.set_vexpand(true);
  progressbar_mem_used_.set_hexpand(true);

  box_ram_.append(progressbar_mem_available_);
  progressbar_mem_available_.set_margin(5);
  progressbar_mem_available_.set_halign(Gtk::Align::CENTER);
  progressbar_mem_available_.set_valign(Gtk::Align::CENTER);
  progressbar_mem_available_.set_show_text(true);
  progressbar_mem_available_.set_size_request(300, -1);
  progressbar_mem_available_.set_vexpand(true);
  progressbar_mem_available_.set_hexpand(true);

  box_ram_.append(progressbar_mem_free_);
  progressbar_mem_free_.set_margin(5);
  progressbar_mem_free_.set_halign(Gtk::Align::CENTER);
  progressbar_mem_free_.set_valign(Gtk::Align::CENTER);
  progressbar_mem_free_.set_show_text(true);
  progressbar_mem_free_.set_size_request(300, -1);
  progressbar_mem_free_.set_vexpand(true);
  progressbar_mem_free_.set_hexpand(true);

  progressbar_mem_tot_.add_css_class("mem-progressbar-tot");
  progressbar_mem_used_.add_css_class("mem-progressbar-used");
  progressbar_mem_available_.add_css_class("mem-progressbar-available");
  progressbar_mem_free_.add_css_class("mem-progressbar-free");

  Glib::signal_timeout().connect([this]()
                                 { return update_mem_usage(); },
                                 1000);

  update_mem_usage();

#if HAVE_NVML || HAVE_ROCM_SMI || HAVE_NVML && HAVE_ROCM_SMI
  frame_gpu_.set_child(box_gpu_);
  frame_gpu_.add_css_class("gpu-frame");

  gpu_data_ = gpu_.get_gpu_data();
  size_t gpu_count = gpu_data_.size();

  for (const auto &g : gpu_data_)
  {
    frame_gpu_.set_label(g.name);
    auto gpu_util = Gtk::make_managed<Gtk::ProgressBar>();
    auto mem_util = Gtk::make_managed<Gtk::ProgressBar>();
    auto tot_vram = Gtk::make_managed<Gtk::ProgressBar>();
    auto used_vram = Gtk::make_managed<Gtk::ProgressBar>();
    auto free_vram = Gtk::make_managed<Gtk::ProgressBar>();

    gpu_util->set_margin(5);
    gpu_util->set_halign(Gtk::Align::CENTER);
    gpu_util->set_valign(Gtk::Align::CENTER);
    gpu_util->set_show_text(true);
    gpu_util->set_size_request(300, -1);
    gpu_util->set_vexpand(true);
    gpu_util->set_hexpand(true);
    gpu_util->add_css_class("gpu-util");

    mem_util->set_margin(5);
    mem_util->set_halign(Gtk::Align::CENTER);
    mem_util->set_valign(Gtk::Align::CENTER);
    mem_util->set_show_text(true);
    mem_util->set_size_request(300, -1);
    mem_util->set_vexpand(true);
    mem_util->set_hexpand(true);
    mem_util->add_css_class("mem-util");

    tot_vram->set_margin(5);
    tot_vram->set_halign(Gtk::Align::CENTER);
    tot_vram->set_valign(Gtk::Align::CENTER);
    tot_vram->set_show_text(true);
    tot_vram->set_size_request(300, -1);
    tot_vram->set_vexpand(true);
    tot_vram->set_hexpand(true);
    tot_vram->add_css_class("tot-vram");

    used_vram->set_margin(5);
    used_vram->set_halign(Gtk::Align::CENTER);
    used_vram->set_valign(Gtk::Align::CENTER);
    used_vram->set_show_text(true);
    used_vram->set_size_request(300, -1);
    used_vram->set_vexpand(true);
    used_vram->set_hexpand(true);
    used_vram->add_css_class("used-vram");

    free_vram->set_margin(5);
    free_vram->set_halign(Gtk::Align::CENTER);
    free_vram->set_valign(Gtk::Align::CENTER);
    free_vram->set_show_text(true);
    free_vram->set_size_request(300, -1);
    free_vram->set_vexpand(true);
    free_vram->set_hexpand(true);
    free_vram->add_css_class("free-vram");

    box_gpu_.append(*gpu_util);
    box_gpu_.append(*mem_util);
    box_gpu_.append(*tot_vram);
    box_gpu_.append(*used_vram);
    box_gpu_.append(*free_vram);

    progressbar_gpu_util_.push_back(gpu_util);
    progressbar_gpu_mem_util_.push_back(mem_util);
    progressbar_gpu_tot_vram_.push_back(tot_vram);
    progressbar_gpu_used_vram_.push_back(used_vram);
    progressbar_gpu_free_vram_.push_back(free_vram);
  }

  Glib::signal_timeout().connect([this]()
                                 { return update_gpu(); },
                                 1000);
  update_gpu();
#endif

  frame_net_.set_child(box_net_);
  frame_net_.set_label(ip_data_.name + " " + ip_data_.addr);
  box_net_.append(progressbar_net_rx_);
  progressbar_net_rx_.set_margin(5);
  progressbar_net_rx_.set_halign(Gtk::Align::CENTER);
  progressbar_net_rx_.set_valign(Gtk::Align::CENTER);
  progressbar_net_rx_.set_size_request(300, -1);
  progressbar_net_rx_.set_show_text(true);
  progressbar_net_rx_.set_vexpand(true);
  progressbar_net_rx_.set_hexpand(true);
  progressbar_net_rx_.add_css_class("net-progressbar-rx");

  box_net_.append(progressbar_net_tx_);
  progressbar_net_tx_.set_margin(5);
  progressbar_net_tx_.set_halign(Gtk::Align::CENTER);
  progressbar_net_tx_.set_valign(Gtk::Align::CENTER);
  progressbar_net_tx_.set_size_request(300, -1);
  progressbar_net_tx_.set_show_text(true);
  progressbar_net_tx_.set_vexpand(true);
  progressbar_net_tx_.set_hexpand(true);
  progressbar_net_tx_.add_css_class("net-progressbar-tx");

  Glib::signal_timeout().connect([this]()
                                 { return update_net_usage(); },
                                 1000);

  frame_fs_.set_child(box_fs_);

  auto mnts = files_system_.get_mounted_file_system();
  for (const auto &mnt : mnts)
  {
    if (mnt.device.find("/dev") == 0 && mnt.fs_type != "tmpfs" && mnt.fs_type != "proc" && mnt.fs_type != "sysfs")
    {
      auto *bar = Gtk::make_managed<Gtk::ProgressBar>();
      bar->set_margin(5);
      bar->set_show_text(true);
      bar->set_halign(Gtk::Align::CENTER);
      bar->set_valign(Gtk::Align::CENTER);
      bar->set_size_request(300, -1);
      bar->set_show_text(true);
      bar->set_vexpand(true);
      bar->set_hexpand(true);
      box_fs_.append(*bar);
      progressbar_fs_.push_back({mnt.mount_point, bar});
    }
  }

  Glib::signal_timeout().connect([this]()
                                 {
    for (const auto& disk : progressbar_fs_)
    {
      update_disk_usage(disk.mntPoint);
    }
    return true; },
                                 1000);

  VBox_.append(frame_cpu_);
  gpu_net_box_.set_spacing(5);
  gpu_net_box_.append(frame_gpu_);
  gpu_net_box_.append(frame_net_);
  VBox_.append(gpu_net_box_);
  ram_fs_box_.set_spacing(5);
  ram_fs_box_.append(frame_ram_);
  ram_fs_box_.append(frame_fs_);
  VBox_.append(ram_fs_box_);

  // key event / change boxes you see
  key_controller_ = Gtk::EventControllerKey::create();

  key_controller_->signal_key_pressed().connect(
    [this](guint keyval, guint keycode, Gdk::ModifierType state) -> bool
    {
      switch (keyval)
      {
        case GDK_KEY_1:
          frame_cpu_.set_visible(!frame_cpu_.get_visible());
          break;
        case GDK_KEY_2:
          frame_ram_.set_visible(!frame_ram_.get_visible());
          break;
        case GDK_KEY_3:
          frame_gpu_.set_visible(!frame_gpu_.get_visible());
          break;
        case GDK_KEY_4:
          frame_fs_.set_visible(!frame_fs_.get_visible());
        case GDK_KEY_5:
          frame_net_.set_visible(!frame_net_.get_visible());
          break;
      }
      return true;
    },
    false);

  add_controller(key_controller_);

  // css
  ref_css_provider_ = Gtk::CssProvider::create();
#if HAS_STYLE_PROVIDER_ADD_PROVIDER_FOR_DISPLAY
  Gtk::StyleProvider::add_provider_for_display(get_display(), m_ref_css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#else
  Gtk::StyleContext::add_provider_for_display(get_display(), ref_css_provider_, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#endif

  ref_css_provider_->signal_parsing_error().connect(
    [](const auto &section, const auto &err)
    { on_parsing_error(section, err); });
  ref_css_provider_->load_from_path("../css/style.css");
}

SystemMonitorWindow::~SystemMonitorWindow()
{
}

bool SystemMonitorWindow::update_cpu_progress_bar()
{
  std::thread([this]()
              {
                prev_ = cpu_.get_cpu_tread_data();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                curr_ = cpu_.get_cpu_tread_data();
                std::vector<CpuPercentage> cpuUsage = cpu_.calculate_cpu_thread_usage(curr_, prev_);

                cpu_usage_data_.clear();
                for (const auto &usage : cpuUsage)
                {
                  cpu_usage_data_.push_back(usage.percentage_used);
                }

                m_dispatcher.emit(); })
    .detach();

  return true;
}

void SystemMonitorWindow::on_parsing_error(const Glib::RefPtr<const Gtk::CssSection> &section, const Glib::Error &error)
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

    std::cerr << "  start_line = " << start_location.get_lines() + 1
              << ", end_line = " << end_location.get_lines() + 1 << "\n";

    std::cerr << "  start_position = " << start_location.get_line_chars()
              << ", end_position = " << end_location.get_line_chars() << "\n";
  }
}

bool SystemMonitorWindow::update_mem_usage()
{
  MemData data = mem_info_.get_mem_data();

  // convert from kb to gb
  double totGIB = data.mem_total / 1024.0 / 1024.0;
  double freeGIB = data.mem_free / 1024.0 / 1024.0;
  double availableGIB = data.mem_available / 1024.0 / 1024.0;
  double usedGIB = totGIB - availableGIB;

  // calculate percentage
  double percentageUsed = usedGIB / totGIB;
  double percentageAvailable = availableGIB / totGIB;
  double percentageFree = freeGIB / totGIB;

  progressbar_mem_tot_.set_text("Total memory: " + two_decimals_format(totGIB) + " GIB");
  progressbar_mem_used_.set_text("Memory used: " + two_decimals_format(usedGIB) + " GIB");
  progressbar_mem_available_.set_text("Available memory: " + two_decimals_format(availableGIB) + " GIB");
  progressbar_mem_free_.set_text("Free memory: " + two_decimals_format(freeGIB) + " GIB");

  progressbar_mem_tot_.set_fraction(1.0);
  progressbar_mem_used_.set_fraction(percentageUsed);
  progressbar_mem_available_.set_fraction(percentageAvailable);
  progressbar_mem_free_.set_fraction(percentageFree);

  return true;
}

#if HAVE_NVML || HAVE_ROCM_SMI || HAVE_NVML && HAVE_ROCM_SMI
bool SystemMonitorWindow::update_gpu()
{
  auto data = gpu_.get_gpu_data();

  for (size_t i = 0; i < data.size() && i < progressbar_gpu_util_.size(); i++)
  {
    const auto &g = data[i];

    progressbar_gpu_util_[i]->set_fraction(g.gpu_util / 100.0);
    progressbar_gpu_util_[i]->set_text("GPU Util: " + two_decimals_format(g.gpu_util) + "%");

    progressbar_gpu_mem_util_[i]->set_fraction(g.mem_util / 100.0);
    progressbar_gpu_mem_util_[i]->set_text("MEM util: " + two_decimals_format(g.mem_util) + "%");

    if (g.tot_vram > 0)
    {
      progressbar_gpu_tot_vram_[i]->set_fraction(g.tot_vram / 100.0);
      progressbar_gpu_tot_vram_[i]->set_text("Tot Vram: " + two_decimals_format(g.tot_vram) + " GIB");

      progressbar_gpu_used_vram_[i]->set_fraction(g.used_vram / 100.0);
      progressbar_gpu_used_vram_[i]->set_text("Used Vram: " + two_decimals_format(g.used_vram) + " GIB " + "/ " + two_decimals_format(g.tot_vram) + " GIB");

      progressbar_gpu_free_vram_[i]->set_text("Free Vram: " + two_decimals_format(g.free_vram) + " GIB " + "/ " + two_decimals_format(g.tot_vram) + " GIB");
      progressbar_gpu_free_vram_[i]->set_fraction(g.free_vram / 100.0);
    }
    else
    {
      progressbar_gpu_tot_vram_[i]->set_fraction(0.0);
      progressbar_gpu_used_vram_[i]->set_fraction(0.0);
      progressbar_gpu_free_vram_[i]->set_fraction(0.0);
    }
  }
  return true;
}
#endif

std::string SystemMonitorWindow::two_decimals_format(double value)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << value;
  return ss.str();
}

bool SystemMonitorWindow::update_net_usage()
{
  static auto prev = net_info_.get_network_stats();
  auto        curr = net_info_.get_network_stats();

  for (const auto &curr_stat : curr)
  {
    if (curr_stat.iface == "lo")
      continue;

    auto prev_stat = std::find_if(prev.begin(), prev.end(), [&curr_stat](const auto &stat)
                                  { return stat.iface == curr_stat.iface; });

    if (prev_stat != prev.end())
    {
      uint64_t rx_diff = curr_stat.rx_bytes - prev_stat->rx_bytes;
      uint64_t tx_diff = curr_stat.tx_bytes - prev_stat->tx_bytes;

      double rx_kb = rx_diff / 1024.0;
      double tx_kb = tx_diff / 1024.0;

      constexpr double maxKbPerSec = 125.0 * 1024.0;

      progressbar_net_rx_.set_text("↓ " + two_decimals_format(rx_kb) + " KB/s");
      progressbar_net_rx_.set_fraction(std::min(rx_kb / maxKbPerSec, 1.0));
      progressbar_net_tx_.set_text("↑ " + two_decimals_format(tx_kb) + " KB/s");
      progressbar_net_tx_.set_fraction(std::min(tx_kb / maxKbPerSec, 1.0));
    }
  }
  prev = std::move(curr);
  return true;
}

bool SystemMonitorWindow::update_disk_usage(const std::string &path)
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

  for (auto &disk : progressbar_fs_)
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
