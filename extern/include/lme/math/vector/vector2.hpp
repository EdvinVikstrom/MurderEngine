#ifndef LIBME_MATH_VECTOR2_HPP
  #define LIBME_MATH_VECTOR2_HPP

#define VECTOR2_OPERATOR(o) \
  Vector2<T> operator o(const Vector2<T> &vec) const \
  { \
    return { \
      x o vec.x, \
      y o vec.y \
    }; \
  } \
  Vector2<T> operator o(const T &val) const \
  { \
    return { \
      x o val, \
      y o val \
    }; \
  }

#define VECTOR2_OPERATOR_ASSIGN(o) \
  Vector2<T>& operator o(const Vector2<T> &vec) \
  { \
    x o vec.x; \
    y o vec.y; \
    return *this; \
  } \
  Vector2<T>& operator o(const T &val) \
  { \
    x o val; \
    y o val; \
    return *this; \
  }

template<typename T>
struct Vector2 {

  T x, y;

  Vector2(T x, T y)
    : x(x), y(y) { }
  Vector2(T v)
    : x(v), y(v) { }
  Vector2() { }

  VECTOR2_OPERATOR(+)
  VECTOR2_OPERATOR(-)
  VECTOR2_OPERATOR(*)
  VECTOR2_OPERATOR(/)

  VECTOR2_OPERATOR_ASSIGN(+=)
  VECTOR2_OPERATOR_ASSIGN(-=)
  VECTOR2_OPERATOR_ASSIGN(*=)
  VECTOR2_OPERATOR_ASSIGN(/=)

  bool operator==(const Vector2<T> &vec) const
  { return x == vec.x && y == vec.y; }
  bool operator!=(const Vector2<T> &vec) const
  { return x != vec.x || y != vec.y; }

  T length() const
  {
    return sqrt(x * x + y * y);
  }

  T distance(const Vector2<T> &vec) const
  {
    T x = this->x - vec.x;
    T y = this->y - vec.y;
    return sqrt(x * x + y * y);
  }

  T dot(const Vector2<T> &vec) const
  {
    return x * vec.x + y * vec.y;
  }

  T angle(const Vector2<T> &vec) const
  {
    float a = x * vec.x + y * vec.y;
    float b = x * vec.x - y * vec.y;
    return atan2(a, b);
  }

};

#undef VECTOR2_OPERATOR
#undef VECTOR2_OPERATOR_ASSIGN

#endif
