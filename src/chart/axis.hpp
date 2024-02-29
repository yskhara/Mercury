#pragma once
#include <gtkmm.h>

class AxisTick {
public:
  // AxisTick();
  AxisTick(const double rel_loc, const std::string &&text);
  // AxisTick(const AxisTick &src);
  // AxisTick(AxisTick &&src);

  double get_rel_pos() const noexcept;
  const std::string &get_text() const noexcept;

  // AxisTick& operator=(const AxisTick &src);
  // AxisTick& operator=(AxisTick &&src);

  std::partial_ordering operator<=>(const AxisTick &other) const noexcept {
    return this->m_rel_pos <=> other.m_rel_pos;
  }

private:
  double m_rel_pos;
  std::string m_text;
};

/**
 * @brief The base class for classes representing an axis in a chart.
 */
class Axis {
public:
  // using DataType = int;
  Axis(Gtk::DrawingArea &parent) : m_parent(parent){};
  // virtual ~Axis(){};
  /**
   * @brief
   *
   * @param cr A Cairo::RefPtr to a Cairo::Context from a RecordingSurface.
   */
  void draw(const Cairo::RefPtr<Cairo::Context> &cr,
            const Glib::RefPtr<Pango::Context> &pg,
            std::vector<AxisTick> &ticks);
  // virtual void set_data_range(DataType data) = 0;
  void set_ticks(std::vector<AxisTick> &&ticks);
  // virtual void
  // estimate_max_ticklabel_dimension(double &crude_max_ticklabel_width,
  //                                  double &crude_max_ticklabel_height) = 0;
  /**
   * @brief Set the max n of divs. Note this may be ignored by some axes
   * whose number of ticks is determined only by data points (e.g.,
   * CategoricalAxis).
   *
   * @param max_n_of_divs
   */
  // virtual void set_max_n_of_divs(int max_n_of_divs) = 0;

  // we don't have to call draw() to determine the axis_length after allocating
  // a new dimention.

  /**
   * @brief Allocate the length to the axis. The length is interpreted as length
   * from the axis origin (or minimum) to the maximum or to the farther end of
   * the maximum tick, whichever is longer.
   * @param length
   * @return true when allocation is not succesful, most likely meaning the
   * allocation is insufficient for the axis and the ticks.
   * @return false when allocation is succesful.
   */
  bool allocate_length(const double length);
  const double get_axis_length() const noexcept;
  const double get_axis_min_offset() const noexcept;
  const double get_axis_max_offset() const noexcept;
  // virtual std::vector<std::string> get_ticks() = 0;
  // virtual std::string get_ticklabel_text_at(D data) = 0;

protected:
  // Axis(BarChartArea &parent);
  Gtk::DrawingArea &m_parent;
  double m_allocated_dimen;
  double m_axis_length;
  double m_axis_min_offset;
  double m_axis_max_offset;
  std::vector<AxisTick> m_ticks;
  std::vector<Glib::RefPtr<Pango::Layout>> m_tick_layouts;
  bool m_tick_layouts_valid = false;

  bool update_layout() noexcept;
};