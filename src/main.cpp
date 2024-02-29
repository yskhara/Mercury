#include <gtkmm.h>
#include <iostream>
#include "ui/ui.hpp"

int main(int argc, char* argv[])
{
  std::cout << "Starting application..." << std::endl;

  auto app = Gtk::Application::create("org.gtkmm.examples.base");
  return app->make_window_and_run<UI::MainWindow>(argc, argv);
}
