#include "include/SystemMonitorWindow.hpp"
#include <gtkmm-4.0/gtkmm/application.h>
#include "include/NvidiaInfo.hpp"

int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("basic.system.monitor");

  NvidiaInfo n;
  n.printNvml();
  
  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
