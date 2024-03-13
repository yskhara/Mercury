#pragma once
#include <gtkmm.h>
#include "axis_range.hpp"
#include "axis.hpp"
#include "dataset.hpp"

namespace Chart {

// TODO: move these into something like "Chart::AxisRenderingOptions"
static constexpr double ChartAreaPadding = 10.0;
/// Minimum distance between Y tick and corresponding tick label
static constexpr double ChartYTickMargin = 2.0;
/// Length of Y tick out of data area
static constexpr double ChartYTickOuterLength = 4.0;
/// Length of Y tick in data area
static constexpr double ChartYTickInnerLength = 4.0;

/// Length of X tick out of data area
static constexpr double ChartXTickOuterLength = 4.0;
/// Length of X tick in data area
static constexpr double ChartXTickInnerLength = 4.0;

static constexpr double ChartTickLabelFontSize = 8.0;

/// Default number of divisions. Used when estimating the optimal ticks / divisions.
static constexpr int ChartDefaultNDiv = 10;

struct ChartDimensions {
  struct Defaults {
  /// Minimum distance between X tick and corresponding tick label
  static constexpr double XTickToLabelMargin = 2.0;
  /// Minimum distance between neighbouring X tick labels
  static constexpr double XTickLabelMargin = 20.0;
  /// Minimum distance between neighbouring Y tick labels
  static constexpr double YTickLabelMargin = 20.0;

  };

  double OuterPadding = ChartAreaPadding;
  /// width of chart area. This is the width of this control minus OuterPadding.
  double ChartAreaWidth;
  double ChartAreaHeight;
  double DataAreaTopMargin;
  double DataAreaLeftMargin;
  double DataAreaHeight;
  double DataAreaWidth;
  double XTickLabelMargin = Defaults::XTickLabelMargin;
  double YTickLabelMargin = Defaults::YTickLabelMargin;
};

template <typename D> class DataPoint {
public:
  std::string to_string();
};

template <typename D>
auto test_if_is_datatype(int)
    -> decltype(void(std::declval<DataPoint<D>>().to_string()),
                std::true_type{});
template <typename D> auto test_if_is_datatype(...) -> std::false_type;

template <typename D>
concept IsDatatype = decltype(test_if_is_datatype<D>(0))::value;

// 軸 (Axis) に関するデータ
//  - 軸の表示オプション (AxisRenderingOptions)
//    * 最小
//    * 最大
//    * 目盛りの刻み (/div)
//    * log / linear
//  - 軸のレンダリング長さ
//  - これだけ？
//  - 
// 軸に関する処理
//  - 軸の最大値および最小値の選択
//  - 軸の目盛り刻みの選択
//  - 軸のレンダリング
//  - 
// GTK+ uses a height-for-width (and width-for-height) geometry management system.
// Height-for-width means that a widget can change how much vertical space it needs, 
// depending on the amount of horizontal space that it is given (and similar for width-for-height). 
// The most common example is a label that reflows to fill up the available width, 
// wraps to fewer lines, and therefore needs less height.

// 軸の長さで正規化して tick の位置を表現するのはどうか？
// position が 0 なら軸の負側の最大位置、 1 なら正側の最大の位置。
// class AxisTick { string TickLabel; double position; }
// これにより、レンダリングに必要な情報は、レンダリング長さと AxisTick のリストだけになる。
// むしろ、全体のサイズ制約を元に、描画時に tick の位置を調整できる。間引くこともできるかも。

// データの中身を見てどうの、という処理は DataSeries
// 側でやる。例えば軸の範囲を決めたりとか。
//  using DataType = double;


template <AxisOrientation O> class AxisRenderer;
class BarChartArea;
#if 0
class GraphConfiguration {
public:
  double axisLineWidth = 1.0;
};

//class AxisRange {};

class DoubleAxisRange {};

class StringAxisRange {};

using CategoricalAxisDataType = std::string;
template <AxisOrientation O>
class CategoricalAxis : virtual public Axis<O>{//<O, CategoricalAxisDataType> {
public:
  //void
  //estimate_max_ticklabel_dimension(double &crude_max_ticklabel_width,
  //                                 double &crude_max_ticklabel_height) override;
  //void set_axis_length(const double length) override;
  //std::vector<std::string> get_ticks() override;
};

/**
 * @brief An axis class representing a continuous axis. (c.f.: discrete axis)
 *
 */
class RealAxis : virtual public Axis {
private:
  using RealAxisDataType = double;
public:
  RealAxis(BarChartArea &parent);
  /**
   * @brief
   *
   * @param cr A Cairo::RefPtr to a Cairo::Context from a RecordingSurface.
   */
  //void draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

  //void set_data_range(RealAxisDataType min, RealAxisDataType max);
  //void set_data_range(DataSeries<RealAxisDataType> data) override;
  //void set_ticks(std::vector<AxisTick> ticks) override;
  //void
  //estimate_max_ticklabel_dimension(double &crude_max_ticklabel_width,
  //                                 double &crude_max_ticklabel_height) override;
  //void set_max_n_of_divs(const int max_n_of_divs) override;
  //void set_axis_length(const double length) override;
  //std::vector<std::string> get_ticks() override;
  //virtual std::string get_ticklabel_text_at(RealAxisDataType data) = 0;

private:
  RealAxisDataType m_axis_min;
  RealAxisDataType m_axis_max;
  RealAxisDataType m_data_min;
  RealAxisDataType m_data_max;

  int m_n_of_divs = 1;
};

template <AxisOrientation O> class AxisRenderer {
public:
  void set_axis_length(const double length);
  void render_axis(const Cairo::RefPtr<Cairo::Context> &cr,
                   const Glib::RefPtr<Pango::Context> &pg,
                   const std::vector<std::string> &ticks);
  void try_render_ticklabel(const Glib::RefPtr<Pango::Context> &pg,
                            const std::string &label_text, int &rendered_width,
                            int &rendered_height);

private:
  double m_axis_length;
};
#endif
/**
 * @brief
 *
 * @tparam Dx X-axis data type.
 * @tparam Dy Y-axis data type.
 */
template <typename Dx, typename Dy>
class TwoDimensionalBarChart : public Gtk::DrawingArea {
public:
  TwoDimensionalBarChart();
  virtual ~TwoDimensionalBarChart();

  void set_chart_data(DataSet<Dx, Dy>&& dataset);
  void optimize_axes_limits();

protected:
  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

private:
  DataSet<Dx, Dy> m_dataset;
  ChartDimensions m_dimensions;

  //DiscreteAxis m_axis_x1;
  Axis<AxisOrientation::Horizontal> m_axis_x;
  Axis<AxisOrientation::Vertical> m_axis_y;
  AxisRange<Dx> m_x_range;
  AxisRange<Dy> m_y_range;
  //RealAxis m_axis_y1;

  void draw_axes(const Cairo::RefPtr<Cairo::Context> &cr);

  void draw_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                      int height);
  void draw_text(const Cairo::RefPtr<Cairo::Context> &cr, int rectangle_width,
                 int rectangle_height);

  void update_chart_area_dimensions();
  void estimate_axis_size();
};

} // namespace Chart

//template<> Chart::TwoDimensionalBarChart<double, double>;
#include "bar_chart_area_impl.hpp"
