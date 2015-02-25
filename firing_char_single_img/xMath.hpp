#ifndef X_MATH_HPP
#define X_MATH_HPP

namespace xMATH {

template<typename Number>
constexpr Number
PI() {
  return Number(3.14159265358979323846);
}

struct Float2 {
  float floats[2];

  Float2()
  {}

  constexpr
  Float2(float x, float y)
    : floats {x, y}
  {}

  // This allows for code which adds a float to a vector. The effect
  // is: `vec += scalar;` is equivalent to `vec += Float(a);`.
  constexpr
  Float2(float a)
    : Float2(a, a)
  {}

  constexpr
  Float2(const float *floats)
    : Float2(floats[0], floats[1])
  {}

  float&
  x() {
    return floats[0];
  }

  float&
  y() {
    return floats[1];
  }

  constexpr float
  x() const {
    return floats[0];
  }

  constexpr float
  y() const {
    return floats[1];
  }

  Float2&
  operator += (Float2 a) {
    floats[0] += a.x();
    floats[1] += a.y();
    return *this;
  }

  Float2&
  operator -= (Float2 a) {
    floats[0] -= a.x();
    floats[1] -= a.y();
    return *this;
  }

  Float2&
  operator *= (float a) {
    floats[0] *= a;
    floats[1] *= a;
    return *this;
  }

  Float2&
  operator /= (float a) {
    return *this *= 1.0f/a;
  }

  Float2&
  negate() {
    floats[0] *= -1.0f;
    floats[1] *= -1.0f;
    return *this;
  }
};

/**
 * Use this at your own risk. Does comparison with == of the underlying
 * floats.
 */
constexpr inline bool
operator == (const Float2 a, const Float2 b) {
  return a.x() == b.x() && a.y() == b.y();
}

constexpr inline float
dot(const Float2 a, const Float2 b) {
  return a.x()*b.x() + a.y()*b.y();
}

constexpr inline float
norm2(const Float2 a) {
  return dot(a, a);
}

inline float
norm(const Float2 a) {
  return std::sqrt(norm2(a));
}

constexpr inline Float2
operator - (const Float2 a) {
  return Float2(-a.x(), -a.y());
}

constexpr inline Float2
operator + (const Float2 a, const Float2 b) {
  return Float2(a.x() + b.x(), a.y() + b.y());
}

constexpr inline Float2
operator - (const Float2 a, const Float2 b) {
  return a + -b;
}

constexpr inline Float2
operator * (float fact, const Float2 a) {
  return Float2(fact*a.x(), fact*a.y());
}

constexpr inline Float2
operator * (const Float2 a, float fact) {
  return fact * a;
}

constexpr inline Float2
operator / (const Float2 a, float fact) {
  return a * (1.0f/fact);
}

inline Float2
normalize(const Float2 a) {
  return Float2(a.x(), a.y())/norm(a);
}

/**
 * Returns a rotation of vector `a` by the angle indicated
 * through the unit direction vector `cos_sin`.
 */
constexpr inline Float2
rotate(const Float2 a, const Float2 cos_sin) {
  // This effectively multiplies a rotation matrix based on cos_sin
  // to the vector a.
  return Float2(cos_sin.x()*a.x() - cos_sin.y()*a.y(),
                cos_sin.y()*a.x() + cos_sin.x()*a.y());
}

}

#endif
