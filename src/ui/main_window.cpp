#include "main_window.hpp"
#include <gtkmm.h>
#include <random>

template class Chart::TwoDimensionalBarChart<double, double>;

UI::MainWindow::MainWindow() {
  set_title("Basic application");
  set_default_size(600, 400);
  set_child(barChartArea);

  auto dataset = Chart::DataSet<double, double>();
  dataset.y.push_back(Chart::DataSeries<double>());
  auto& data_y = dataset.y.at(0);
  //auto data_x = std::vector<double>();
  //auto data_y = std::vector<double>();
  std::random_device seedGen;
  std::default_random_engine engine(seedGen());
  std::uniform_int_distribution<> uniformDist(0, 4200);
  for (int i = 400; i < 600; i++) {
    dataset.x.push_back(i);
    data_y.push_back(uniformDist(engine));
  }
  barChartArea.set_chart_data(std::move(dataset));

  barChartArea.set_expand();
}

void UI::MainWindow::Initialize() {
}
