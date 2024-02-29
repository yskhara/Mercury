#pragma once
#include "chart/chart.hpp"
#include <gtkmm.h>

namespace UI {
class MainWindow : public Gtk::ApplicationWindow
{
public:
    MainWindow();

    void Initialize();

protected:
    Chart::TwoDimensionalBarChart<double, double> barChartArea;

};
}
