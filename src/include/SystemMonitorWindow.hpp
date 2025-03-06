#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/progressbar.h>

class SystemMonitorWindow : public Gtk::Window
{
protected:
  Gtk::Box m_HBox;
  Gtk::Box m_VBox;
  Gtk::Box m_box_cpu, m_box_gpu, m_box_ram;
  Gtk::Frame m_frame_cpu, m_frame_gpu, m_frame_ram;
  Gtk::ProgressBar m_progressbar_cpu, m_progressbar_gpu, m_progressbar_ram;

public:
  SystemMonitorWindow();
  ~SystemMonitorWindow();
};

