#pragma once

#include "MemInfo.hpp"
#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/progressbar.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <glibmm/dispatcher.h>
#include <vector>
#include "CpuUsage.hpp"
#include "glibmm/error.h"
#include "glibmm/refptr.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/csssection.h"
#include "NvidiaInfo.hpp"
#include "NetInfo.hpp"

class SystemMonitorWindow : public Gtk::Window
{
private:
  Gtk::Box m_HBox;
  Gtk::Box m_VBox;
  Gtk::Grid m_grid_cpu;
  Gtk::Box m_box_cpu, m_box_gpu, m_box_ram, m_box_net, m_ram_gpu_box;
  Gtk::Frame m_frame_cpu, m_frame_gpu, m_frame_ram, m_frame_net;
  Gtk::ProgressBar m_progressbar_gpu_nvidia_gpuUtil, m_progressbar_gpu_nvidia_memUtil, m_progressbar_gpu_nvidia_totVram, m_progressbar_gpu_nvidia_usedVram, m_progressbar_gpu_nvidia_freeVram, m_progressbar_mem_tot, m_progressbar_mem_used, m_progressbar_mem_available, m_progressbar_mem_free;
  std::vector<Gtk::ProgressBar*> m_progressbar_cpu;

  static void on_parsing_error(const Glib::RefPtr<const Gtk::CssSection>& section, const Glib::Error& error);
  
  // CPU
  CpuUsage cpu;
  std::vector<CpuData> prev;
  std::vector<CpuData> curr;

  bool update_cpu_progress_bar();

  Glib::Dispatcher m_dispatcher;
  std::vector<double> cpuUsageData;

  Glib::RefPtr<Gtk::CssProvider> m_ref_css_provider;

  // mem

  MemInfo memInfo;
  
  bool update_mem_usage();

  // Nvidia GPU
  NvidiaInfo nvidia;

  bool update_nvidia_gpu_usage();

  // net
  NetInfo netInfo;
  IpData ipData;

  // formating
  std::string two_decimals_format(double value);
  
public:
  SystemMonitorWindow();
  ~SystemMonitorWindow();
};

