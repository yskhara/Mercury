#include "axis.hpp"
#include "bar_chart_area.hpp"

AxisTick::AxisTick(const double rel_pos, const std::string &&text)
    : m_rel_pos(rel_pos), m_text(text) {}
double AxisTick::get_rel_pos() const noexcept { return m_rel_pos; }
const std::string &AxisTick::get_text() const noexcept { return m_text; }

template <AxisOrientation Orientation>
void Axis<Orientation>::draw(const Cairo::RefPtr<Cairo::Context> &cr,
                             const Glib::RefPtr<Pango::Context> &pg) {
  auto layout = Pango::Layout::create(pg);

  // try to draw the last ticklabel
  // auto tick_last = m_ticks.back();
  int text_width, text_height;

  // layout->set_text(tick_last.get_text());
  // layout->get_pixel_size(text_width, text_height);

  /// FIXME: ちゃんと長さ計算するように。
  // double axis_length =
  //     (m_allocated_dimen - (text_width / 2.0)) / tick_last.get_rel_pos();
  // axis_length = m_axis_length;

  // 毎回 ticks は描画し直す。これでよい。
  for (const auto &tick : m_ticks) {
    layout->set_text(tick.get_text());
    layout->get_pixel_size(text_width, text_height);
    double tick_center_x = m_axis_length * tick.get_rel_pos();
    cr->move_to(tick_center_x, 0);
    cr->line_to(tick_center_x, Chart::ChartXTickOuterLength);
    cr->set_line_width(1.0);
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->stroke();

    /// FIXME: Do not use default values
    cr->move_to(tick_center_x - (text_width / 2.0),
                Chart::ChartXTickOuterLength +
                    Chart::ChartDimensions::Defaults::XTickToLabelMargin);
    layout->show_in_cairo_context(cr);
  }

  cr->move_to(0.0, 0.0);
  cr->line_to(m_axis_length, 0.0);
  cr->set_line_width(1.0);
  cr->set_source_rgb(0.0, 0.0, 0.0);
  cr->stroke();

  // m_axis_length = axis_length;
}

template <AxisOrientation Orientation>
void Axis<Orientation>::set_ticks(std::vector<AxisTick> &&ticks) {
  m_ticks = ticks;
  std::sort(m_ticks.begin(), m_ticks.end());

  m_tick_layouts_valid = false;
}

// ただの setter 関数。
template <AxisOrientation Orientation>
bool Axis<Orientation>::allocate_new_length(const double length) {
  m_allocated_dimen = length;
  //update_layout();
  //update_axis_length();
  return false;
}

template <AxisOrientation Orientation>
const double Axis<Orientation>::get_axis_length() const noexcept {
  return m_axis_length;
}

template <AxisOrientation Orientation>
const double Axis<Orientation>::get_axis_hanging_begin() const noexcept {
  return m_axis_hanging_begin;
}

template <AxisOrientation Orientation>
double Axis<Orientation>::get_axis_hanging_end() const noexcept {
  return m_axis_hanging_end;
}

/// FIXME: このメンバ関数を使ってやりたかったことは、 Ticks
/// を最適化するために試しに描画させてみることだったはず。であれば、
/// try_draw_ticks
/// のようなメンバ関数を用意してそいつにやらせるべきだったのではないか？
/// 今の実装では m_tick_layouts に pango layout を格納しているが、 draw()
/// が呼ばれた時点の Pango context
/// で描画し直すべきなので、キャッシュとしても使えないため微妙。
template <AxisOrientation Orientation>
void Axis<Orientation>::get_ticks_worst_dimensions(
    double &ticklabel_dim_max,
    double &ticklabel_center_dist_min) const noexcept {
  if (!m_tick_layouts_valid) {
    return;
  }

  auto ticks_count = m_ticks.size();
  if (ticks_count < 2 || m_tick_layouts.size() != ticks_count) {
    return;
  }

  int tick_width, tick_height;
  m_tick_layouts[0]->get_pixel_size(tick_width, tick_height);
  // if it is an x-axis:
  double _ticklabel_dim_max = tick_width;
  double _ticklabel_center_dist_min = std::numeric_limits<double>::max();

  for (auto i = 0; i < ticks_count - 1; i++) {
    auto ticks_center_dist =
        std::abs(m_axis_length *
                 (m_ticks[i + 1].get_rel_pos() - m_ticks[i].get_rel_pos()));
    // if it is an x-axis:
    int tick_next_width, tick_next_height;
    m_tick_layouts[i + 1]->get_pixel_size(tick_next_width, tick_next_height);

    if (ticks_center_dist < _ticklabel_center_dist_min) {
      _ticklabel_center_dist_min = ticks_center_dist;
    }

    if (tick_next_width > _ticklabel_dim_max) {
      _ticklabel_dim_max = tick_next_width;
    }
  }

  ticklabel_dim_max = _ticklabel_dim_max;
  ticklabel_center_dist_min = _ticklabel_center_dist_min;
}

template <AxisOrientation Orientation>
bool Axis<Orientation>::update_layout() noexcept {
  // no need to update layout.
  if (m_tick_layouts_valid)
    return false;

  m_tick_layouts.clear();
  for (const auto &tick : m_ticks) {
    /// TODO: add support for rotated texts
    m_tick_layouts.push_back(m_parent.create_pango_layout(tick.get_text()));
  }

  m_tick_layouts_valid = true;
  return false;
}

template <AxisOrientation Orientation>
bool Axis<Orientation>::update_axis_length() noexcept {
  if (m_ticks.empty() || m_tick_layouts.empty()) {
    m_axis_length = 0;
    m_axis_hanging_begin = 0;
    m_axis_hanging_end = 0;
    return false;
  }

  int end_tick_natural_width, end_tick_natural_height;
  m_tick_layouts.back()->get_pixel_size(end_tick_natural_width,
                                        end_tick_natural_height);

  double axis_len = 2.0 * m_allocated_dimen /
                    (3.0 * (m_ticks.end() - 1)->get_rel_pos() -
                     (m_ticks.end() - 2)->get_rel_pos());
  if (axis_len > m_allocated_dimen) {
    axis_len = m_allocated_dimen;
  }

  // ホントはこの長さでいいのか確認してから設定するべき。
  m_axis_length = axis_len;
  m_axis_hanging_end = (end_tick_natural_width / 2.0) -
                       (m_axis_length * (1.0 - m_ticks.end()->get_rel_pos()));
  return false;
}

template class Axis<AxisOrientation::Horizontal>;
template class Axis<AxisOrientation::Vertical>;
