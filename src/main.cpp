#include "include/SystemMonitorWindow.hpp"
#include <gtkmm-4.0/gtkmm/application.h>


int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("basic.system.monitor");
  
  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
