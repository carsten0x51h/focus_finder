#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_

#include <limits>
#include <cmath>  /* for std::abs(double) */

// https://stackoverflow.com/questions/19837576/comparing-floating-point-number-to-zero
// see Knuth section 4.2.2 pages 217-218
template <typename T>
static bool isAlmostEqual(T x, T y) {
  return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x);
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_*/
