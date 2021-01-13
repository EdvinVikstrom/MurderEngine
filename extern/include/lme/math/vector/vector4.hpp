#ifndef LIBME_MATH_VECTOR4_HPP
  #define LIBME_MATH_VECTOR4_HPP

template<typename T>
struct Vector2;
template<typename T>
struct Vector3;


#define VECTOR4_OPERATOR(o) \
  Vector4<T> operator o(const Vector4<T> &vec) const \
  { \
    return { \
      x o vec.x, \
      y o vec.y, \
      z o vec.z, \
      w o vec.w \
    }; \
  } \
  Vector4<T> operator o(const T &val) const \
  { \
    return { \
      x o val, \
      y o val, \
      z o val, \
      w o val \
    }; \
  }

#define VECTOR4_OPERATOR_ASSIGN(o) \
  Vector4<T>& operator o(const Vector4<T> &vec) \
  { \
    x o vec.x; \
    y o vec.y; \
    z o vec.z; \
    w o vec.w; \
    return *this; \
  } \
  Vector4<T>& operator o(const T &val) \
  { \
    x o val; \
    y o val; \
    z o val; \
    w o val; \
    return *this; \
  }

template<typename T>
struct Vector4 {

  T x, y, z, w;

  Vector4(T x, T y, T z, T w)
    : x(x), y(y), z(z), w(w) { }
  Vector4(T v)
    : x(v), y(v), z(v), w(v) { }
  Vector4(const Vector2<T> &vec)
    : x(vec.x), y(vec.y), z(0), w(0) { }
  Vector4(const Vector3<T> &vec)
    : x(vec.x), y(vec.y), z(vec.z), w(0) { }
  Vector4() { }

  Vector2<T> vec2()
  { return Vector2<T>(x, y); }
  Vector3<T> vec3()
  { return Vector3<T>(x, y, z); }

  VECTOR4_OPERATOR(+)
  VECTOR4_OPERATOR(-)
  VECTOR4_OPERATOR(*)
  VECTOR4_OPERATOR(/)

  VECTOR4_OPERATOR_ASSIGN(+=)
  VECTOR4_OPERATOR_ASSIGN(-=)
  VECTOR4_OPERATOR_ASSIGN(*=)
  VECTOR4_OPERATOR_ASSIGN(/=)

  bool operator==(const Vector4<T> &vec) const
  { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; }
  bool operator!=(const Vector4<T> &vec) const
  { return x != vec.x || y != vec.y || z != vec.z || w != vec.w; }

};

#undef VECTOR4_OPERATOR
#undef VECTOR4_OPERATOR_ASSIGN

#endif
