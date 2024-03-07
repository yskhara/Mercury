#pragma once
#include "bar_chart_area.hpp"
#include <cmath>
#include <format>
#include <gtkmm.h>
#include <iostream>
#include <stdexcept>

using namespace Chart;

template <typename Dx, typename Dy>
TwoDimensionalBarChart<Dx, Dy>::TwoDimensionalBarChart() {
  // m_axis_y1 =
  // std::make_unique<NumericalAxis<AxisOrientation::Vertical>>(*this);
  // m_axis_y2 =
  // std::make_unique<NumericalAxis<AxisOrientation::Vertical>>(*this);

  m_dimensions.OuterPadding = ChartAreaPadding;

  set_draw_func(sigc::mem_fun(*this, &TwoDimensionalBarChart::on_draw));
}

template <typename Dx, typename Dy>
TwoDimensionalBarChart<Dx, Dy>::~TwoDimensionalBarChart() {}

template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::set_chart_data(DataSet<Dx, Dy> &&dataset) {
  m_dataset = dataset;
  // obtain y value
  // FIXME: this isn't right.
  /*std::vector<DataType> data_y1;
  for(auto &data : *m_data){
    data_y1.push_back(data.second);
  }
  m_axis_y1->set_data_range(data_y1);*/
}

template <typename Dx, typename Dy>
std::tuple<double, double>
TwoDimensionalBarChart<Dx, Dy>::get_chart_area_dimensions() {
  return {get_width() - (2 * ChartAreaPadding),
          get_height() - (2 * ChartAreaPadding)};
}

template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::optimize_axes_limits() {
  auto [chart_area_x, chart_area_y] = get_chart_area_dimensions();

  m_dataset.autorange_x(m_x_range);
  m_dataset.autorange_y(m_y_range);

  std::vector<AxisTick> ticks;
  ticks.push_back(AxisTick(0.0, std::format("{}", m_x_range.min)));
  ticks.push_back(
      AxisTick(0.5, std::format("{}", (m_x_range.min + m_x_range.min) / 2)));
  ticks.push_back(AxisTick(1.0, std::format("{}", m_x_range.max)));

  double _x_ticklabel_dim_along_max = 0;
  double _x_ticklabel_dim_perp_max = 0;
  m_axis_x.try_draw(get_pango_context(), ticks,
                    _x_ticklabel_dim_along_max, _x_ticklabel_dim_perp_max);

  auto x_n_div_max =
      m_axis_x.get_axis_length() /
      (_x_ticklabel_dim_along_max + m_dimensions.XTickLabelMargin);

  m_dataset.autorange_x(m_x_range, x_n_div_max);

  std::cout << std::format(
                   "min: {}, max: {}, n_div: {} (requested: {}), tick_step: {}",
                   m_x_range.min, m_x_range.max, m_x_range.n_div, x_n_div_max,
                   (m_x_range.max - m_x_range.min) / m_x_range.n_div)
            << std::endl;

  // find candidate ticklabel size
  double cand_x_ticklabel_width = 0.0, cand_x_ticklabel_height = 0.0,
         cand_y_ticklabel_width = 0.0, cand_y_ticklabel_height = 0.0;
  // m_axis_x1->estimate_max_ticklabel_dimension(cand_x_ticklabel_width,
  //                                               cand_x_ticklabel_height);
  // m_axis_y1->estimate_max_ticklabel_dimension(cand_y_ticklabel_width,
  //                                               cand_y_ticklabel_height);

  double cand_chart_data_area_width =
      chart_area_x - (cand_y_ticklabel_width + m_dimensions.YTickLabelMargin +
                      ChartYTickOuterLength + (cand_x_ticklabel_width / 2.0));
  double cand_chart_data_area_height =
      chart_area_y - (cand_x_ticklabel_height + m_dimensions.XTickLabelMargin +
                      ChartXTickOuterLength + (cand_y_ticklabel_height / 2.0));
  std::cout << "cand_chart_data_area_width : " << cand_chart_data_area_width
            << std::endl
            << "cand_chart_data_area_height : " << cand_chart_data_area_height
            << std::endl;

  // find maximum number of divisions
  int max_n_of_divs_x =
      cand_chart_data_area_width /
      (cand_x_ticklabel_width + m_dimensions.XTickLabelMargin);
  int max_n_of_divs_y =
      cand_chart_data_area_height /
      (cand_y_ticklabel_height + m_dimensions.YTickLabelMargin);
  // m_axis_y1->set_max_n_of_divs(max_n_of_divs_y);
  // m_axis_y1->set_axis_length(cand_chart_data_area_height);

  m_dimensions.DataAreaWidth = cand_chart_data_area_width;
  m_dimensions.DataAreaHeight = cand_chart_data_area_height;
  m_dimensions.DataAreaLeftMargin = cand_y_ticklabel_width +
                                    m_dimensions.YTickLabelMargin +
                                    ChartYTickOuterLength;
  m_dimensions.DataAreaTopMargin = cand_y_ticklabel_height / 2.0;
}

/**
 * @brief Draws all the axes. For each axis object, let the object draw the axis
 * on a cairo's RecordingSurface, and then replay the recorded surface on
 * chart's surface with setting transform for each axis accordingly.
 * @param cr A cairo object to draw axes on.
 */
template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::draw_axes(
    const Cairo::RefPtr<Cairo::Context> &cr) {
  auto recSurf = Cairo::RecordingSurface::create();
  auto recContext = Cairo::Context::create(recSurf);

  auto ticks = std::vector<AxisTick>();
  for (int i = 0; i < m_x_range.n_div + 1; i++) {
    auto r = 1.0 / m_x_range.n_div * i;
    ticks.push_back(AxisTick(
        r, std::format("{}",
                       m_x_range.min + (m_x_range.max - m_x_range.min) * r)));
  }

  m_axis_x.set_ticks(std::move(ticks));
  // m_axis_x.allocate_length(chart_area_x);

  // m_axis_x.set_ticks(std::move(ticks));
  m_axis_x.allocate_new_length(
      m_dimensions.DataAreaWidth); // get_width());// - (2 *
                                   // m_dimensions.OuterPadding));
  m_axis_x.draw(recContext, get_pango_context());

  auto axis_extents = recSurf->ink_extents();
  cr->set_source(
      recSurf, m_dimensions.OuterPadding + m_dimensions.DataAreaLeftMargin,
      m_dimensions.OuterPadding + m_dimensions.DataAreaTopMargin +
          m_dimensions.DataAreaHeight - (axis_extents.y + axis_extents.height));
  cr->paint();

  recSurf = Cairo::RecordingSurface::create();
  recContext = Cairo::Context::create(recSurf);
  auto y_ticks = std::vector<AxisTick>();
  for (int i = 0; i < m_y_range.n_div + 1; i++) {
    auto r = 1.0 / m_y_range.n_div * i;
    y_ticks.push_back(AxisTick(
        r, std::format("{}",
                       m_y_range.min + (m_y_range.max - m_y_range.min) * r)));
  }

  m_axis_y.set_ticks(std::move(y_ticks));
  m_axis_y.allocate_new_length(m_dimensions.DataAreaHeight);
  // get_width());// - (2 * m_dimensions.OuterPadding));
  m_axis_y.draw(recContext, get_pango_context());
  axis_extents = recSurf->ink_extents();
  cr->set_source(
      recSurf, m_dimensions.OuterPadding + m_dimensions.DataAreaLeftMargin,
      m_dimensions.OuterPadding + m_dimensions.DataAreaTopMargin +
          m_dimensions.DataAreaHeight - (axis_extents.y + axis_extents.height));
  cr->paint();
}

template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::on_draw(
    const Cairo::RefPtr<Cairo::Context> &cr, int width, int height) {
  const int rectangle_width = width;
  const int rectangle_height = height / 2;

  optimize_axes_limits();
  draw_axes(cr);
  return;

  // Draw a black rectangle
  cr->set_source_rgb(0.0, 0.0, 0.0);
  draw_rectangle(cr, rectangle_width, rectangle_height);

  // and some white text
  cr->set_source_rgb(1.0, 1.0, 1.0);
  draw_text(cr, rectangle_width, rectangle_height);

  // flip the image vertically
  // see
  // http://www.cairographics.org/documentation/cairomm/reference/classCairo_1_1Matrix.html
  // the -1 corresponds to the yy part (the flipping part)
  // the height part is a translation (we could have just called
  // cr->translate(0, height) instead) it's height and not height / 2, since we
  // want this to be on the second part of our drawing (otherwise, it would draw
  // over the previous part)
  Cairo::Matrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, height);

  // apply the matrix
  cr->transform(matrix);

  // white rectangle
  cr->set_source_rgb(1.0, 1.0, 1.0);
  draw_rectangle(cr, rectangle_width, rectangle_height);

  // black text
  cr->set_source_rgb(0.0, 0.0, 0.0);
  draw_text(cr, rectangle_width, rectangle_height);
}

template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::draw_rectangle(
    const Cairo::RefPtr<Cairo::Context> &cr, int width, int height) {
  cr->rectangle(0, 0, width, height);
  cr->fill();
}

template <typename Dx, typename Dy>
void TwoDimensionalBarChart<Dx, Dy>::draw_text(
    const Cairo::RefPtr<Cairo::Context> &cr, int rectangle_width,
    int rectangle_height) {
  // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
  Pango::FontDescription font;

  font.set_family("Monospace");
  font.set_weight(Pango::Weight::BOLD);

  // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
  auto layout = create_pango_layout("Hi there!");

  layout->set_font_description(font);

  int text_width;
  int text_height;

  // get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);

  // Position the text in the middle
  cr->move_to((rectangle_width - text_width) / 2.0,
              (rectangle_height - text_height) / 2.0);

  layout->show_in_cairo_context(cr);
}
