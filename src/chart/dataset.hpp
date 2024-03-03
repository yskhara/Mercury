#pragma once

#include "axis_range.hpp"
#include <vector>

namespace Chart {
/**
 * @brief One-dimensional data series.
 * FIXME: It shall be possible to store, say, std::string to a DataSeries, which
 * is currently not.
 */
template <typename D> // requires IsDatatype<D>
using DataSeries = std::vector<D>;
// class DataSeries : public std::vector<D> {
//   static void find_suitable_axis_range(D);
// };

template <typename DataType> // requires IsDatatype<D>
class DataSetAutorangeWorker {
public:
  static bool autorange(AxisRange<DataType> &range, const DataType min,
                        const DataType max);
  static bool autorange(AxisRange<DataType> &range, const DataType min,
                        const DataType max, const unsigned int n_div_max);
};

/**
 * @brief Two-dimensional data series.
 *
 * @tparam Dx
 * @tparam Dy
 * @tparam N
 */
template <typename Dx, typename Dy> // requires IsDatatype<D>
class DataSet {
public:
  // using DataSet = std::pair<std::vector<Dx>, std::vector<Dy>>;
  DataSeries<Dx> x;
  std::vector<DataSeries<Dy>> y;

  /**
   * @brief Find an optimum set of X-axis range options (i.e., min, max) based
   * on the dataset without constraint on the number of divisions.
   * @param x_range An AxisRange<D> object to store the range options.
   * @return true Indicates an error.
   * @return false Indicates a successful auto-ranging.
   */
  bool autorange_x(AxisRange<Dx> &x_range);
  /**
   * @brief Find an optimum set of X-axis range options (i.e., min, max, and
   * number of divisions) based on the dataset.
   * @param x_range An AxisRange<D> object to store the range options.
   * @param n_div_max The maximum number of divisions to be used as a constraint.
   * @return true Indicates an error.
   * @return false Indicates a successful auto-ranging.
   */
  bool autorange_x(AxisRange<Dx> &x_range, const unsigned int n_div_max);

  /**
   * @brief Find an optimum set of Y-axis range options (i.e., min, max) based
   * on the dataset without constraint on the number of divisions.
   * @param y_range An AxisRange<D> object to store the range options.
   * @return true Indicates an error.
   * @return false Indicates a successful auto-ranging.
   */
  bool autorange_y(AxisRange<Dy> &y_range);

  /**
   * @brief Find an optimum set of Y-axis range options (i.e., min, max, and
   * number of divisions) based on the dataset.
   * @param y_range An AxisRange<D> object to store the range options.
   * @param n_div_max The maximum number of divisions to be used as a constraint.
   * @return true Indicates an error.
   * @return false Indicates a successful auto-ranging.
   */
  bool autorange_y(AxisRange<Dy> &y_range, const unsigned int n_div_max);

private:
  bool find_minmax_x(Dx &min, Dx &max);
  bool find_minmax_y(Dy &min, Dy &max);
};
// using DataSet = std::vector<DataSeries>;
} // namespace Chart

#include "dataset_impl.hpp"
