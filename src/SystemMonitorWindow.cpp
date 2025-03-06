#include "include/SystemMonitorWindow.hpp"
#include "gtkmm/enums.h"

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

  m_frame_cpu.set_child(m_box_cpu);
  m_box_cpu.append(m_progressbar_cpu);
  m_progressbar_cpu.set_margin(5);
  m_progressbar_cpu.set_halign(Gtk::Align::CENTER);
  m_progressbar_cpu.set_valign(Gtk::Align::CENTER);
  m_progressbar_cpu.set_size_request(100, -1);
  m_progressbar_cpu.set_text("CPU progress bar");
  m_progressbar_cpu.set_show_text(true);

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
}

SystemMonitorWindow::~SystemMonitorWindow() {}
