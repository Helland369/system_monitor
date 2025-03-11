#pragma once

#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/progressbar.h>

#include "CpuUsage.hpp"

class SystemMonitorWindow : public Gtk::Window
{
private:
  Gtk::Box m_HBox;
  Gtk::Box m_VBox;
  Gtk::Box m_box_cpu, m_box_gpu, m_box_ram;
  Gtk::Frame m_frame_cpu, m_frame_gpu, m_frame_ram;
  Gtk::ProgressBar m_progressbar_cpu, m_progressbar_gpu, m_progressbar_ram;

  // CPU
  CpuUsage cpu;
  CpuData prev; 
  CpuData curr;

  bool update_cpu_progress_bar();

public:
  SystemMonitorWindow();
  ~SystemMonitorWindow();
};

