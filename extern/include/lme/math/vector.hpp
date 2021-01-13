#ifndef LIBME_MATH_VECTOR_HPP
  #define LIBME_MATH_VECTOR_HPP

#include "math.hpp"

namespace me::math {

#include "vector/vector2.hpp"
#include "vector/vector3.hpp"
#include "vector/vector4.hpp"

  typedef Vector2<float> vec2f;
  typedef Vector3<float> vec3f;
  typedef Vector4<float> vec4f;

  typedef Vector2<int> vec2i;
  typedef Vector3<int> vec3i;
  typedef Vector4<int> vec4i;

} /* end namespace me::math */

#endif
