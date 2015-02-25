#ifndef X_MATH_H
#define X_MATH_H

#include <math.h>

#define XM_PI_f 3.141592f
#define XM_HPI_f (0.5f*XM_PI_f)
#define XM_3HPI_f (1.5f*XM_PI_f)
#define XM_2PI_f (2.0f*XM_PI_f)

#define XM_PI_d 3.141592653589793
#define XM_HPI_d (0.5*XM_PI_d)
#define XM_3HPI_d (1.5*XM_PI_d)
#define XM_2PI_d (2.0*XM_PI_d)

#define XM_PI_ld 3.14159265358979323846l
#define XM_HPI_ld (0.5l*XM_PI_ld)
#define XM_3HPI_ld (1.5l*XM_PI_ld)
#define XM_2PI_ld (2.0l*XM_PI_ld)


/**
 * This is not an abstract type. The existence of a typedef is an exception of
 * the rule I follow (i.e. typedef'ing out a struct, union or whatever else
 * marks abstract types). Typing struct XM_Float2 everywhere is just too
 * annoying, it also makes lines long and requires vertical spacing to fit
 * things into small lines.
 *
 * Similarly, functions in this module aren't prefixed. For example, add_2f
 * is used instead of XM_Add_2f.
 */
struct vec2f {
  float x, y;
};

typedef struct vec2f vec2f;

static inline vec2f*
addto_2f(vec2f *a, vec2f b) {
  a->x += b.x;
  a->y += b.y;
  return a;
}

static inline vec2f*
subto_2f(vec2f *a, vec2f b) {
  a->x -= b.x;
  a->y -= b.y;
  return a;
}

static inline vec2f*
multo_2f(vec2f *a, float x) {
  a->x *= x;
  a->y *= x;
  return a;
}

static inline vec2f*
divto_2f(vec2f *a, float x) {
  return multo_2f(a, 1.0f/x);
}

static inline vec2f*
negto_2f(vec2f *a) {
  a->x *= -1.0f;
  a->y *= -1.0f;
  return a;
}

static inline float
dot_2f(vec2f a, vec2f b) {
  return a.x*b.x + a.y*b.y;
}

static inline float
norm2_2f(vec2f a) {
  return dot_2f(a, a);
}

static inline float
norm_2f(vec2f a) {
  return sqrtf(norm2_2f(a));
}

static inline vec2f
sub_2f(vec2f a, vec2f b) {
  return *subto_2f(&a, b);
}

static inline vec2f
add_2f(vec2f a, vec2f b) {
  return *addto_2f(&a, b);
}

static inline vec2f
neg_2f(vec2f a) {
  return *negto_2f(&a);
}

static inline vec2f
mul_2f(vec2f a, float x) {
  return *multo_2f(&a, x);
}

static inline vec2f
div_2f(vec2f a, float x) {
  return *divto_2f(&a, x);
}

static inline vec2f*
normalizeto_2f(vec2f *a) {
  return divto_2f(a, norm_2f(*a));
}

static inline vec2f
normalize_2f(vec2f a) {
  return *normalizeto_2f(&a);
}

/**
 * Returns a rotation of vector `a` by the angle indicated
 * through the unit direction vector `cos_sin`.
 */
static inline vec2f*
rotto_2f(vec2f *a, vec2f cos_sin) {
  // This effectively multiplies a rotation matrix based on cos_sin
  // to the vector a.
  float x = cos_sin.x*a->x - cos_sin.y*a->y;
  a->y = cos_sin.y*a->x + cos_sin.x*a->y;
  a->x = x;
  return a;
}

static inline vec2f
rot_2f(vec2f a, vec2f cos_sin) {
  return *rotto_2f(&a, cos_sin);
}

/**
 * Use at your own rist. This does memberwise comparison with the == operator.
 */
static inline int
eq_2f(vec2f a, vec2f b) {
  return a.x == b.x && a.y == b.y;
}

#endif
