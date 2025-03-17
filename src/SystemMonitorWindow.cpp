#include "include/SystemMonitorWindow.hpp"
#include "CpuUsage.hpp"
#include "glibmm/error.h"
#include "glibmm/main.h"
#include "glibmm/refptr.h"
#include "gtk/gtk.h"
#include "gtkmm/csssection.h"
#include "gtkmm/enums.h"
#include "gtkmm/object.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/stylecontext.h"
#include <gtkmm/cssprovider.h>
#include <cstddef>
#include <glibmm.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

SystemMonitorWindow::SystemMonitorWindow()
    : m_VBox(Gtk::Orientation::VERTICAL, 5),
      m_HBox(Gtk::Orientation::HORIZONTAL, 5), m_frame_cpu("CPU"),
      m_frame_ram("RAM"), m_frame_gpu("GPU"),
      m_box_cpu(Gtk::Orientation::VERTICAL, 5),
      m_box_gpu(Gtk::Orientation::VERTICAL, 5),
      m_box_ram(Gtk::Orientation::VERTICAL, 5)      
{
  // set title for the wondow
  set_title("System Monitor");
  // set window size
  set_default_size(800, 800);
  
  m_HBox.set_margin(5);
  set_child(m_HBox);
  m_HBox.append(m_VBox);

  prev = cpu.get_cpu_tread_data();
  size_t cpu_count = prev.size();

  for (size_t i = 0; i < cpu_count; i++)
  {
    auto prog_bar = Gtk::make_managed<Gtk::ProgressBar>();
    m_progressbar_cpu.push_back(prog_bar);
    
    prog_bar->set_margin(5);
    prog_bar->set_halign(Gtk::Align::CENTER);
    prog_bar->set_valign(Gtk::Align::CENTER);
    prog_bar->set_size_request(100, -1);
    prog_bar->set_text("CPU" + std::to_string(i));
    prog_bar->set_show_text(true);

    m_frame_cpu.set_child(m_box_cpu);
    m_box_cpu.append(*prog_bar);
  }

  m_dispatcher.connect([this]()
  {
    for (size_t i = 0; i < cpuUsageData.size() && i < m_progressbar_cpu.size(); i++)
    {
      double usageFraction = cpuUsageData[i] / 100.0;
      m_progressbar_cpu[i]->set_fraction(usageFraction);
      m_progressbar_cpu[i]->add_css_class("cpu-progress-bar");
      m_progressbar_cpu[i]->set_size_request(300, -1);
      m_progressbar_cpu[i]->set_vexpand(true);
      m_progressbar_cpu[i]->set_hexpand(true);

      if (i == 0)
      {
        m_progressbar_cpu[0]->set_text("Total CPU " + std::to_string(cpuUsageData[0]) + "%");
      }
      else
      {
        m_progressbar_cpu[i]->set_text("CPU" + std::to_string(i) + " " + std::to_string(cpuUsageData[i]) + "%");
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
  
  m_frame_gpu.set_child(m_box_gpu);
  m_box_gpu.append(m_progressbar_gpu);
  m_progressbar_gpu.set_margin(5);
  m_progressbar_gpu.set_halign(Gtk::Align::CENTER);
  m_progressbar_gpu.set_valign(Gtk::Align::CENTER);
  m_progressbar_gpu.set_size_request(100, -1);
  m_progressbar_gpu.set_text("GPU progress bar");
  m_progressbar_gpu.set_show_text(true);

  m_frame_ram.set_child(m_box_ram);
  m_box_ram.append(m_progressbar_ram);
  m_progressbar_ram.set_margin(5);
  m_progressbar_ram.set_halign(Gtk::Align::CENTER);
  m_progressbar_ram.set_valign(Gtk::Align::CENTER);
  m_progressbar_ram.set_size_request(100, -1);
  m_progressbar_ram.set_text("RAM progress bar");
  m_progressbar_ram.set_show_text(true);
  
  m_VBox.append(m_frame_cpu);
  m_VBox.append(m_frame_gpu);
  m_VBox.append(m_frame_ram);

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
      cpuUsageData.push_back(usage.persentageUsed);
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
