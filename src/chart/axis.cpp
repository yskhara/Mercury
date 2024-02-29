#include "axis.hpp"
#include "bar_chart_area.hpp"

AxisTick::AxisTick(const double rel_pos, const std::string &&text)
    : m_rel_pos(rel_pos), m_text(text) {}

#if 0
AxisTick::AxisTick(const AxisTick &src)
    : m_rel_pos(src.m_rel_pos), m_text(src.m_text) {}

AxisTick::AxisTick(AxisTick &&src)
    : m_rel_pos(src.m_rel_pos), m_text(std::move(src.m_text)) {}

AxisTick &AxisTick::operator=(const AxisTick &src) {
  if (&src != this) {
    m_rel_pos = src.m_rel_pos;
    m_text = src.m_text;
  }
  return *this;
}

AxisTick &AxisTick::operator=(AxisTick &&src) {
  if (&src != this) {
    m_rel_pos = src.m_rel_pos;
    m_text = std::move(src.m_text);
  }
  return *this;
}
#endif

double AxisTick::get_rel_pos() const noexcept { return m_rel_pos; }
const std::string &AxisTick::get_text() const noexcept { return m_text; }

void Axis::draw(const Cairo::RefPtr<Cairo::Context> &cr,
                const Glib::RefPtr<Pango::Context> &pg,
                std::vector<AxisTick> &ticks) {
  auto layout = Pango::Layout::create(pg);

  // try to draw the last ticklabel
  auto tick_iter = std::prev(ticks.end());
  int text_width, text_height;

  layout->set_text(tick_iter->get_text());
  layout->get_pixel_size(text_width, text_height);
  double axis_length =
      (m_allocated_dimen - (text_width / 2.0)) / tick_iter->get_rel_pos();

  for (; tick_iter != (ticks.begin() - 1); --tick_iter) {
    layout->set_text(tick_iter->get_text());
    layout->get_pixel_size(text_width, text_height);
    double tick_center_x = axis_length * tick_iter->get_rel_pos();
    cr->move_to(tick_center_x, 0);
    cr->line_to(tick_center_x, Chart::ChartXTickOuterLength);
    cr->set_line_width(1.0);
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->stroke();

    cr->move_to(tick_center_x - (text_width / 2.0),
                Chart::ChartXTickOuterLength + Chart::ChartXTickLabelMargin);
    layout->show_in_cairo_context(cr);
  }

  cr->move_to(0.0, 0.0);
  cr->line_to(axis_length, 0.0);
  cr->set_line_width(1.0);
  cr->set_source_rgb(0.0, 0.0, 0.0);
  cr->stroke();

  m_axis_length = axis_length;
}

void Axis::set_ticks(std::vector<AxisTick> &&ticks) {
  m_ticks = ticks;
  std::sort(m_ticks.begin(), m_ticks.end());

  m_tick_layouts_valid = false;
}

bool Axis::allocate_length(const double length) {
  m_allocated_dimen = length;
  return true;

  auto tick_first = m_ticks.front(), tick_last = m_ticks.back();
  int text_width, text_height;

  auto layout = m_parent.create_pango_layout(tick_last.get_text());

  layout->get_pixel_size(text_width, text_height);
  m_axis_length =
      (m_allocated_dimen - (text_width / 2.0)) / tick_last.get_rel_pos();

  m_axis_max_offset =
      (text_width / 2.0) - (m_axis_length * (1.0 - tick_last.get_rel_pos()));

  return false;
}

const double Axis::get_axis_length() const noexcept { return m_axis_length; }

const double Axis::get_axis_min_offset() const noexcept {
  return m_axis_min_offset;
}

const double Axis::get_axis_max_offset() const noexcept {
  return m_axis_max_offset;
}

/// FIXME: should this be marked as noexcept?
bool Axis::update_layout() noexcept {
  // no need to update layout.
  if (m_tick_layouts_valid)
    return false;

  m_tick_layouts.clear();
  for (auto &tick : m_ticks) {
    /// TODO: add support for rotated texts
    m_tick_layouts.push_back(m_parent.create_pango_layout(tick.get_text()));
  }

  int end_tick_natural_width, end_tick_natural_height;
  m_tick_layouts.back()->get_pixel_size(end_tick_natural_width, end_tick_natural_height);

  double end_tick_width;

  /// FIXME: this code is assuming that there are more than one ticks
  double axis_len = 2.0 * m_allocated_dimen /
                    (3.0 * (m_ticks.end() - 1)->get_rel_pos() -
                     (m_ticks.end() - 2)->get_rel_pos());
  if (axis_len > m_allocated_dimen) {
    axis_len = m_allocated_dimen;
  }

  const AxisTick &tick_first = *m_ticks.begin();
  const AxisTick &tick_last = *std::prev(m_ticks.end());

  for (auto tick_iter = m_ticks.begin(); tick_iter < m_ticks.end();
       ++tick_iter) {
    auto layout = m_parent.create_pango_layout(tick_iter->get_text());
    auto max_width = std::numeric_limits<int>::max();
    if (tick_iter != m_ticks.begin()) {
    }
  }
/*
  layout->get_pixel_size(text_width, text_height);
  m_axis_length =
      (m_allocated_dimen - (text_width / 2.0)) / tick_last.get_rel_pos();

  m_axis_max_offset =
      (text_width / 2.0) - (m_axis_length * (1.0 - tick_last.get_rel_pos()));
*/
  return false;
}
