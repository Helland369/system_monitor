#include "include/SystemMonitorWindow.hpp"
#include <gtkmm-4.0/gtkmm/application.h>
#include "include/FileSystem.hpp"

int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("basic.system.monitor");

  FileSystem f;
  auto m = f.get_mounted_file_system();
  for (const auto &mm : m)
    {
    if (mm.device.find("/dev") == 0)
        {
          f.print_filesystem(mm.mountPoint);
        }
    }    
  
  return app->make_window_and_run<SystemMonitorWindow>(argc, argv);
}
