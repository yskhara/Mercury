#pragma once
#include "bar_chart_area.hpp"
#include <cmath>
#include <format>
#include <gtkmm.h>
#include <iostream>
#include <stdexcept>

using namespace Chart;

#if 0
template <AxisOrientation O>
Axis<O>::Axis(TwoDimensionalBarChart &parent) : m_parent(parent) {}

template <AxisOrientation O>
void RealAxisAxis<O>::draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  this->m_renderer.render_axis(cr, this->m_parent.get_pango_context(),
                               get_ticks());
}

template <AxisOrientation O>
RealAxisAxis<O>::RealAxisAxis(TwoDimensionalBarChart &parent) : Axis<O>(parent) {}

template <AxisOrientation O>
void RealAxisAxis<O>::set_data_range(DataType min, DataType max) {
  m_data_min = min;
  m_data_max = max;

  // calculate crude axis limits
  auto max_exp = std::floor(std::log10(m_data_max));
  m_axis_max =
      std::ceil(m_data_max / std::pow(10.0, max_exp)) * pow(10.0, max_exp);

  auto min_exp = std::floor(std::log10(-m_data_min));
  m_axis_min =
      -std::ceil(-m_data_min / std::pow(10, min_exp)) * pow(10.0, min_exp);

  // TODO: make this conditional; not all axis requre zero to be within its
  // limits
  if (m_data_max > 0) {
    if (!(m_data_min < 0)) {
      m_axis_min = 0.0;
    }
  } else {
    if (m_data_min < 0) {
      m_axis_max = 0.0;
    }
  }
}

template <AxisOrientation O>
void RealAxisAxis<O>::set_data_range(std::vector<DataType> data) {
  return set_data_range(*std::min_element(data.begin(), data.end()),
                        *std::max_element(data.begin(), data.end()));
}

template <AxisOrientation O>
void RealAxisAxis<O>::estimate_max_ticklabel_dimension(
    double &width, double &height) {
  
  auto ticklabels_to_try = std::vector<std::string>

  // calculate crude axis limits
  auto max_exp = std::floor(std::log10(m_data_max));
  double max_cand =
      std::ceil(m_data_max / std::pow(10.0, max_exp)) * pow(10.0, max_exp);

  auto min_exp = std::floor(std::log10(-m_data_min));
  double min_cand =
      -std::ceil(-m_data_min / std::pow(10, min_exp)) * pow(10.0, min_exp);

  // TODO: make this conditional; not all axis requre zero to be within its
  // limits
  if (m_data_max > 0) {
    if (!(m_data_min < 0)) {
      min_cand = 0.0;
    }
  } else {
    if (m_data_min < 0) {
      max_cand = 0.0;
    }
  }

  auto pango_context = this->m_parent.get_pango_context();
  int text_max_width, text_max_height, text_min_width, text_min_height;

  this->m_renderer.try_render_ticklabel(pango_context,
                                        std::format("{}", max_cand),
                                        text_max_width, text_max_height);
  this->m_renderer.try_render_ticklabel(pango_context,
                                        std::format("{}", min_cand),
                                        text_min_width, text_min_height);

  crude_max_ticklabel_width =
      text_min_width < text_max_width ? text_max_width : text_min_width;
  crude_max_ticklabel_height =
      text_min_height < text_max_height ? text_max_height : text_min_height;
}

template <AxisOrientation O>
void RealAxisAxis<O>::set_max_n_of_divs(const int max_n_of_divs) {
  m_n_of_divs = max_n_of_divs;
  //m_axis_min = m_data_min;
  //m_axis_max = m_data_max;
}

template <AxisOrientation O>
void RealAxisAxis<O>::set_axis_length(const double length) {
  this->m_renderer.set_axis_length(length);
}

template <AxisOrientation O>
std::vector<std::string> RealAxisAxis<O>::get_ticks() {
  auto ticks = std::vector<std::string>(m_n_of_divs + 1);
  for (int i = 0; i < m_n_of_divs + 1; i++) {
    ticks[i] = std::format("{}", m_axis_min + (m_axis_max - m_axis_min) * i /
                                                     m_n_of_divs);
  }
  return ticks;
}

template <AxisOrientation O>
void AxisRenderer<O>::render_axis(const Cairo::RefPtr<Cairo::Context> &cr,
                                  const Glib::RefPtr<Pango::Context> &pg,
                                  const std::vector<std::string> &ticks) {
  throw std::logic_error("not implemented");
}
#endif

#if 0
template <AxisOrientation O>
void AxisRenderer<O>::try_render_ticklabel(
    const Glib::RefPtr<Pango::Context> &pg, std::string const &label_text,
    int &rendered_width, int &rendered_height) {
  auto font_desc = pg->get_font_description();
  // font_desc.set_size(8);

  // get size for ticklabel
  auto layout = Pango::Layout::create(pg);
  layout->set_text(label_text);
  layout->set_font_description(font_desc);
  // FIXME: maybe we should use get_size() instead of get_pixel_size
  layout->get_pixel_size(rendered_width, rendered_height);
}
#endif

template <typename Dx, typename Dy> // requires IsDatatype<D>
void DataSet<Dx, Dy>::autorange(AxisRange<Dx> &x_range,
                                AxisRange<Dy> &y_range) {
  if (this->x.size() > 0) {
    Dx x_min, x_max;
    x_min = x_max = this->x[0];
    for (auto const &val : this->x) {
      if (val < x_min) {
        x_min = val;
      } else if (val > x_max) {
        x_max = val;
      }
    }

    Dx x_delta = x_max - x_min;
    Dx x_delta_pow = std::pow(10, std::floor(std::log10(x_delta)));
    x_range.max = std::ceil(x_max / x_delta_pow) * x_delta_pow;
    x_range.min = std::floor(x_min / x_delta_pow) * x_delta_pow;
  }
}

template <typename Dx, typename Dy>
TwoDimensionalBarChart<Dx, Dy>::TwoDimensionalBarChart()
    : m_axis_x(*this), m_axis_y(*this) {
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

  m_dataset.autorange(m_x_range, m_y_range);

  std::vector<AxisTick> ticks;
  ticks.push_back(AxisTick(0.0, std::format("{}", m_x_range.min)));
  ticks.push_back(AxisTick(0.5, std::format("{}", (m_x_range.min + m_x_range.min) / 2)));
  ticks.push_back(AxisTick(1.0, std::format("{}", m_x_range.max)));

  double _x_ticklabel_dim_max = 0;
  double _x_ticklabel_distance_min = 0;
  m_axis_x.set_ticks(std::move(ticks));
  m_axis_x.allocate_length(chart_area_x);
  m_axis_x.get_ticks_worst_dimensions(_x_ticklabel_dim_max, _x_ticklabel_distance_min);

  std::cout << _x_ticklabel_dim_max << ", " << _x_ticklabel_distance_min << std::endl;

  // find candidate ticklabel size
  double cand_x_ticklabel_width = 0.0, cand_x_ticklabel_height = 0.0,
         cand_y_ticklabel_width = 0.0, cand_y_ticklabel_height = 0.0;
  // m_axis_x1->estimate_max_ticklabel_dimension(cand_x_ticklabel_width,
  //                                               cand_x_ticklabel_height);
  // m_axis_y1->estimate_max_ticklabel_dimension(cand_y_ticklabel_width,
  //                                               cand_y_ticklabel_height);

  double cand_chart_data_area_width =
      chart_area_x - (cand_y_ticklabel_width + ChartYTickLabelMargin +
                      ChartYTickOuterLength + (cand_x_ticklabel_width / 2.0));
  double cand_chart_data_area_height =
      chart_area_y - (cand_x_ticklabel_height + ChartXTickLabelMargin +
                      ChartXTickOuterLength + (cand_y_ticklabel_height / 2.0));
  std::cout << "cand_chart_data_area_width : " << cand_chart_data_area_width
            << std::endl
            << "cand_chart_data_area_height : " << cand_chart_data_area_height
            << std::endl;

  // find maximum number of divisions
  int max_n_of_divs_x = cand_chart_data_area_width /
                        (cand_x_ticklabel_width + ChartXTickLabelMargin);
  int max_n_of_divs_y = cand_chart_data_area_height /
                        (cand_y_ticklabel_height + ChartYTickLabelMargin);
  // m_axis_y1->set_max_n_of_divs(max_n_of_divs_y);
  // m_axis_y1->set_axis_length(cand_chart_data_area_height);

  m_dimensions.DataAreaWidth = cand_chart_data_area_width;
  m_dimensions.DataAreaHeight = cand_chart_data_area_height;
  m_dimensions.DataAreaLeftMargin =
      cand_y_ticklabel_width + ChartYTickLabelMargin + ChartYTickOuterLength;
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
  for (int i = 0; i <= 10; i++) {
    auto val = m_x_range.min + (m_x_range.max - m_x_range.min) * i / 10.0;
    ticks.push_back(AxisTick(i / 10.0, std::format("{}", val)));
  }

  m_axis_x.set_ticks(std::move(ticks));
  m_axis_x.allocate_length(
      m_dimensions.DataAreaWidth); // get_width());// - (2 *
                                   // m_dimensions.OuterPadding));
  m_axis_x.draw(recContext, get_pango_context());

  auto axis_extents = recSurf->ink_extents();
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
