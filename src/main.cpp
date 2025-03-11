#include "include/SystemMonitorWindow.hpp"
#include <gtkmm-4.0/gtkmm/application.h>
// #include <iostream>
// #include <thread>
// #include <chrono>
// #include "include/CpuUsage.hpp"

int main(int argc, char* argv[])
{
  // CpuUsage cpu;
  // CpuData prev = cpu.get_cpu_data();
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // CpuData curr = cpu.get_cpu_data();

  // double cpuUsage = cpu.calculate_cpu_usage(prev, curr);
  // std::cout << "CPU usage: " << cpuUsage << "%\n";

  auto app = Gtk::Application::create("basic.system.monitor");

  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
