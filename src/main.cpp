#include "include/SystemMonitorWindow.hpp"
#include <cstddef>
#include <gtkmm-4.0/gtkmm/application.h>
#include <iostream>
#include "CpuUsage.hpp"


int main(int argc, char* argv[])
{
  CpuUsage cpu;
  
  std::vector<CpuData> cpu_data = cpu.get_cpu_tread_data();

  for (size_t i = 0; i < cpu_data.size(); i++)
  {
    std::cout << cpu_data[i].label << cpu_data[i].user << "\n";
  }

  auto app = Gtk::Application::create("basic.system.monitor");
  
  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
