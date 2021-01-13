#ifndef LIBME_MATH_HPP
  #define LIBME_MATH_HPP

#include "../type.hpp"

#include <cmath>

namespace me::math {

  inline double invsqrt(double v)
  {
    return 1.0 / ::sqrt(v);
  }

  inline float invsqrtf(float v)
  {
    return 1.0F / ::sqrt(v);
  }

  inline size_t max(size_t a, size_t b)
  {
    return a > b ? a : b;
  }

  inline size_t min(size_t a, size_t b)
  {
    return a < b ? a : b;
  }

}

#endif
