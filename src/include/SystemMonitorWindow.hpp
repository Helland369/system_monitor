#pragma once

#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/progressbar.h>
#include <memory>
#include <glibmm/dispatcher.h>
#include <vector>

#include "CpuUsage.hpp"

class SystemMonitorWindow : public Gtk::Window
{
private:
  Gtk::Box m_HBox;
  Gtk::Box m_VBox;
  Gtk::Box m_box_cpu, m_box_gpu, m_box_ram;
  Gtk::Frame m_frame_cpu, m_frame_gpu, m_frame_ram;
  Gtk::ProgressBar m_progressbar_gpu, m_progressbar_ram;
  std::vector<Gtk::ProgressBar*> m_progressbar_cpu;
  
  // CPU
  CpuUsage cpu;
  std::vector<CpuData> prev;
  std::vector<CpuData> curr;

  bool update_cpu_progress_bar();

  Glib::Dispatcher m_dispatcher;
  std::vector<double> cpuUsageData;
  
public:
  SystemMonitorWindow();
  ~SystemMonitorWindow();
};

