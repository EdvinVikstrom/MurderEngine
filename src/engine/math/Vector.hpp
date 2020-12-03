#ifndef ME_VECTOR_HPP
  #define ME_VECTOR_HPP

#include "Maths.hpp"

namespace me {

  template<typename T>
  struct Vector2 {

    T x, y;

    Vector2()
    {
      x = 0;
      y = 0;
    }

    Vector2(T x, T y)
    {
      this->x = x;
      this->y = y;
    }

  };

  template<typename T>
  struct Vector3 {

    T x, y, z;

    Vector3()
    {
      x = 0;
      y = 0;
      z = 0;
    }

    Vector3(T x, T y, T z)
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }

  };

  template<typename T>
  struct Vector4 {

    T x, y, z, w;

    Vector4()
    {
      x = 0;
      y = 0;
      z = 0;
      w = 0;
    }

    Vector4(T x, T y, T z, T w)
    {
      this->x = x;
      this->y = y;
      this->z = z;
      this->w = w;
    }

  };

  typedef Vector2<int> vec2i;
  typedef Vector3<int> vec3i;
  typedef Vector4<int> vec4i;

  typedef Vector2<long> vec2l;
  typedef Vector3<long> vec3l;
  typedef Vector4<long> vec4l;

  typedef Vector2<ME_FLOAT> vec2f;
  typedef Vector3<ME_FLOAT> vec3f;
  typedef Vector4<ME_FLOAT> vec4f;

}

#endif
