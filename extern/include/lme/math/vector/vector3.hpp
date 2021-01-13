#ifndef LIBME_MATH_VECTOR3_HPP
  #define LIBME_MATH_VECTOR3_HPP

template<typename T>
struct Vector2;


#define VECTOR3_OPERATOR(o) \
  Vector3<T> operator o(const Vector3<T> &vec) const \
  { \
    return { \
      x o vec.x, \
      y o vec.y, \
      z o vec.z \
    }; \
  } \
  Vector3<T> operator o(const T &val) const \
  { \
    return { \
      x o val, \
      y o val, \
      z o val \
    }; \
  }

#define VECTOR3_OPERATOR_ASSIGN(o) \
  Vector3<T>& operator o(const Vector3<T> &vec) \
  { \
    x o vec.x; \
    y o vec.y; \
    z o vec.z; \
    return *this; \
  } \
  Vector3<T>& operator o(const T &val) \
  { \
    x o val; \
    y o val; \
    z o val; \
    return *this; \
  }

template<typename T>
struct Vector3 {

  T x, y, z;

  Vector3(T x, T y, T z)
    : x(x), y(y), z(z) { }
  Vector3(T v)
    : x(v), y(v), z(v) { }
  Vector3(const Vector2<T> &vec)
    : x(vec.x), y(vec.y), z(0) { }
  Vector3() { }

  Vector2<T> vec2()
  { return Vector2<T>(x, y); }

  VECTOR3_OPERATOR(+)
  VECTOR3_OPERATOR(-)
  VECTOR3_OPERATOR(*)
  VECTOR3_OPERATOR(/)

  VECTOR3_OPERATOR_ASSIGN(+=)
  VECTOR3_OPERATOR_ASSIGN(-=)
  VECTOR3_OPERATOR_ASSIGN(*=)
  VECTOR3_OPERATOR_ASSIGN(/=)

  bool operator==(const Vector3<T> &vec) const
  { return x == vec.x && y == vec.y && z == vec.z; }
  bool operator!=(const Vector3<T> &vec) const
  { return x != vec.x || y != vec.y || z != vec.z; }

};

#undef VECTOR3_OPERATOR
#undef VECTOR3_OPERATOR_ASSIGN

#endif
