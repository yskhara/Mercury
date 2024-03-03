#pragma once
#include "dataset.hpp"
#include <cmath>

using namespace Chart;

template <typename Dx, typename Dy> // requires IsDatatype<D>
bool DataSet<Dx, Dy>::autorange_x(AxisRange<Dx> &x_range) {
  Dx x_min, x_max;
  if (find_minmax_x(x_min, x_max)) {
    return true;
  }
  return DataSetAutorangeWorker<Dx>::autorange(x_range, x_min, x_max);
}

template <typename Dx, typename Dy>
bool DataSet<Dx, Dy>::autorange_y(AxisRange<Dy> &y_range) {
  /// FIXME: this should be unnecessary
  if (this->y.size() < 1) {
    return true;
  }
  Dy y_min, y_max;
  if (find_minmax_y(y_min, y_max)) {
    return true;
  }

  return DataSetAutorangeWorker<Dy>::autorange(y_range, y_min, y_max);
}

template <typename Dx, typename Dy>
bool DataSet<Dx, Dy>::autorange_x(AxisRange<Dx> &x_range,
                                  const unsigned int n_div_max) {
  Dx x_min, x_max;
  if (find_minmax_x(x_min, x_max)) {
    return true;
  }

  return DataSetAutorangeWorker<Dx>::autorange(x_range, x_min, x_max, n_div_max);
}

template <typename Dx, typename Dy>
bool DataSet<Dx, Dy>::find_minmax_x(Dx &min, Dx &max) {
  if (this->x.size() < 1) {
    return true;
  }

  Dx _min, _max;
  _min = _max = this->x[0];
  for (auto const &val : this->x) {
    if (val < _min) {
      _min = val;
    } else if (val > _max) {
      _max = val;
    }
  }
  min = _min;
  max = _max;
  return false;
}

template <typename Dx, typename Dy>
bool DataSet<Dx, Dy>::find_minmax_y(Dy &min, Dy &max) {
  Dy _min, _max;
  bool is_minmax_valid = false;
  for (const auto &series : this->y) {
    if (series.size() < 1) {
      continue;
    }

    if (!is_minmax_valid) {
      _min = _max = series[0];
      is_minmax_valid = true;
    }

    for (auto const &val : series) {
      if (val < _min) {
        _min = val;
      } else if (val > _max) {
        _max = val;
      }
    }
  }

  if (!is_minmax_valid) {
    return true;
  }

  min = _min;
  max = _max;
  return false;
}

template <typename DataType>
bool DataSetAutorangeWorker<DataType>::autorange(AxisRange<DataType> &range,
                                                 const DataType min,
                                                 const DataType max) {
  DataType x_delta = max - min;
  DataType x_delta_pow = std::pow(10, std::floor(std::log10(x_delta)));
  range.max = std::ceil(max / x_delta_pow) * x_delta_pow;
  range.min = std::floor(min / x_delta_pow) * x_delta_pow;
  range.n_div = 1;

  return false;
}

template <typename DataType>
bool DataSetAutorangeWorker<DataType>::autorange(AxisRange<DataType> &range,
                                                 const DataType min,
                                                 const DataType max,
                                                 const unsigned int n_div_max) {

  DataType delta = max - min;
  DataType delta_pow = std::pow(10, std::floor(std::log10(delta)));
  DataType crude_max = std::ceil(max / delta_pow) * delta_pow;
  DataType crude_min = std::floor(min / delta_pow) * delta_pow;

  const double tick_steps_to_try[] = {10, 5, 2.5, 2, 1};
  double min_tick_step = delta / (double)n_div_max;
  double min_tick_step_pow =
      std::pow(10, std::floor(std::log10(min_tick_step)));
  double min_tick_step_msd = min_tick_step / min_tick_step_pow;
  double candidate_tick_step_msd = tick_steps_to_try[0];
  for (auto &tick_step : tick_steps_to_try) {
    if (min_tick_step_msd < tick_step && tick_step < candidate_tick_step_msd) {
      candidate_tick_step_msd = tick_step;
    }
  }
  double tick_step = candidate_tick_step_msd * min_tick_step_pow;
  int range_max_tick = std::ceil(crude_max / tick_step);
  int range_min_tick = std::floor(crude_min / tick_step);
  range.max = range_max_tick * tick_step;
  range.min = range_min_tick * tick_step;
  range.n_div = range_max_tick - range_min_tick;

  return false;
}
