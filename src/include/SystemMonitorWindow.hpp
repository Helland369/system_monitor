#pragma once

#include "MemInfo.hpp"
#include "CpuUsage.hpp"
#include "NetInfo.hpp"
#include "FileSystem.hpp"
#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/progressbar.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/eventcontrollerkey.h>
#include "gtkmm/cssprovider.h"
#include "gtkmm/csssection.h"
#include "gtkmm/widget.h"
#include <glibmm/dispatcher.h>
#include "glibmm/error.h"
#include "glibmm/refptr.h"
#include <vector>

#if HAVE_NVML || HAVE_ROCM_SMI
#include "GpuInfo.hpp"
#endif

class DiskUsage
{
public:
  std::string       mntPoint;
  Gtk::ProgressBar *bar;
};

class SystemMonitorWindow : public Gtk::Window
{
private:
  Gtk::Box         HBox_;
  Gtk::Box         VBox_;
  Gtk::Grid        grid_cpu_;
  Gtk::Box         box_cpu_, box_gpu_, box_ram_, box_net_, box_fs_, ram_fs_box_, gpu_net_box_;
  Gtk::Frame       frame_cpu_, frame_gpu_, frame_ram_, frame_net_, frame_fs_;
  Gtk::ProgressBar progressbar_gpu_nvidia_gpuUtil_,
    progressbar_mem_tot_, progressbar_mem_used_,
    progressbar_mem_available_, progressbar_mem_free_, progressbar_net_rx_,
    progressbar_net_tx_;

  std::vector<Gtk::ProgressBar *> progressbar_cpu_,
    progressbar_gpu_util_,
    progressbar_gpu_mem_util_,
    progressbar_gpu_tot_vram_,
    progressbar_gpu_used_vram_,
    progressbar_gpu_free_vram_;
  std::vector<DiskUsage> progressbar_fs_;

  // key events
  Glib::RefPtr<Gtk::EventControllerKey> key_controller_;

  // css
  static void on_parsing_error(const Glib::RefPtr<const Gtk::CssSection> &section, const Glib::Error &error);

  // CPU
  CpuUsage             cpu_;
  std::vector<CpuData> prev_;
  std::vector<CpuData> curr_;

  bool update_cpu_progress_bar();

  Glib::Dispatcher    m_dispatcher;
  std::vector<double> cpu_usage_data_;

  Glib::RefPtr<Gtk::CssProvider> ref_css_provider_;

  // mem
  MemInfo mem_info_;

  bool update_mem_usage();

#if HAVE_NVML || HAVE_ROCM_SMI || HAVE_NVML && HAVE_ROCM_SMI
  GpuInfo   gpu_;
  std::vector<GpuData> gpu_data_;
  bool      update_gpu();
#endif

  // net
  NetInfo net_info_;
  IpData  ip_data_;
  bool    update_net_usage();

  // disks / file-system
  FileSystem files_system_;
  bool       update_disk_usage(const std::string &path);

  // formatting
  std::string two_decimals_format(double value);

public:
  SystemMonitorWindow();
  ~SystemMonitorWindow();
};
