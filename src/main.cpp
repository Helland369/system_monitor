#include "include/SystemMonitorWindow.hpp"
#include <cstddef>
#include <gtkmm-4.0/gtkmm/application.h>
#include <iostream>
#include "CpuUsage.hpp"
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
  CpuUsage cpu;
  
  std::vector<CpuData> cpu_prev = cpu.get_cpu_tread_data();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::vector<CpuData> cpu_curr = cpu.get_cpu_tread_data();

  std::vector<CpuPercentage> p_cpu = cpu.calculate_cpu_thread_usage(cpu_curr, cpu_prev);
  //p_cpu.push_back(cpu.calculate_cpu_thread_usage(cpu_curr, cpu_prev));

  for (size_t i = 0; i < p_cpu.size(); i++) {
    std::cout << p_cpu[i].name << " " << p_cpu[i].persentageUsed << "\n";
  }

  auto app = Gtk::Application::create("basic.system.monitor");
  
  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
