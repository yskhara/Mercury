#pragma once

namespace Chart {
template <typename DataValueType> class AxisRange {
public:
  DataValueType min;
  DataValueType max;
  unsigned int n_div;

  DataValueType get_tick_step() const { return (max - min) / n_div; }
};
} // namespace Chart
