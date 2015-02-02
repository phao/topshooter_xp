#ifndef SHARED_H
#define SHARED_H

/**
 * The idea here is to attempt for the first time to put "common things
 * together" for the topshooter code.
 *
 * As of now, it contains basic type definitions, and also drawing and
 * positioning functions. Some important assumptions are also baked into
 * here:
 *
 * - Things should be in floating point values (float) as much as possible.
 *
 * - Images are assumed to be pointing right with their base point at
 * their mid-left and their top point at their mid-right. This implies for
 * example that their horizontal extent is actually their height! It also
 * implies that their vertical extent is actually their width!
 *
 * - Drawing an image involves specifying a rotation around its center, and
 * the position of its center in the destination screen (it also involves
 * specifying more things, but these 2 are the most important).
 *
 * - The drawing functions will always rotate images around their center.
 * There are no exceptions to this.
 *
 * - You should stick to CenterFor<Base|Top>At functions for calculating
 * positions of where to draw images.
 *
 * - Draw<DIR>Image assumes the following coordinate system: bottom-left screen
 * point is the origin; y grows up; x grows right.
 *
 * - If your image doesn't point right, you should manipulate it as if it
 * did and then call the right drawing function when drawing (e.g. if your
 * image points down, you should call DrawDownImage). If you have an image
 * which naturally points at 55 degrees, what you're going to do is treat it
 * as if it pointed right and you'll draw it with something like
 * DrawLeftImage(..., -55 degrees in radians, ...).
 *
 * Treating images this way can be quite confusing if you're doing all the
 * coordinates transformations yourself, so you should try to as much as
 * possible think in terms of Draw<DIR>Image and CenterFor<Base|Top>At. While
 * doing *your own* manipulations, if you stick to the assumption that images
 * point right and so forth, things are actually very straightforward. The
 * complication comes in if you try to bypass Draw<DIR>Image and/or
 * CenterFor<Base|Top>At.
 *
 * Once you know the rotation (around its center) the image should have and
 * its base (or top) point, you should call CenterFor<Base|Top>At to get
 * the values for the center point. Then you should call Draw<DIR>Image for
 * drawing.
 *
 * CenterFor<Base|Top>At is basically your positioning routine. With the
 * given scheme for dealing with images, it's quite difficult to figure out
 * by head or even on paper where everything should go. Thinking in terms of
 * (1) base points for images, (2) top points for images, (3) images pointing
 * right, and (4) coordinate systems where y-positive is up makes things
 * extremely easier. This happens because, then, given you draw your image
 * correctly (i.e. with the right Draw<DIR>Image call), all you need to
 * know are things like:
 *
 * - What is the center point (which Draw<DIR>Image takes) so that the base or
 * top point is <such> for a center rotation of <so much>?
 *
 * This question is basically what CenterFor<Base|Top>At answers. It lets you
 * give it the base or top point you'd like to have, the rotation you'd like
 * to have for the image, and it spills you out the center point you should
 * pass to Draw<DIR>Image together with the rotation value so the image gets
 * drawn as you'd expect.
 */

#include <cmath>
#include <utility>
#include <stdexcept>

#include <iostream>

#include "xSDL.hpp"
#include "xSDL_image.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SHR {

struct Float2 {
  float floats[2];

  Float2()
  {}

  Float2(float x, float y) {
    floats[0] = x;
    floats[1] = y;
  }

  // This allows for code which adds a float to a vector. The effect
  // is: `vec += scalar;` is equivalent to `vec += Float(a);`.
  Float2(float a)
    : Float2(a, a)
  {}

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

  const float&
  x() const {
    return floats[0];
  }

  const float&
  y() const {
    return floats[1];
  }

  Float2&
  operator += (const Float2& a) {
    floats[0] += a.x();
    floats[1] += a.y();
    return *this;
  }

  Float2&
  operator -= (const Float2& a) {
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
inline bool
operator == (const Float2& a, const Float2& b) {
  return a.x() == b.x() && a.y() == b.y();
}

inline float
dot(const Float2& a, const Float2& b) {
  return a.x()*b.x() + a.y()*b.y();
}

inline float
norm2(const Float2& a) {
  return dot(a, a);
}

inline float
norm(const Float2& a) {
  return std::sqrt(norm2(a));
}

inline Float2
operator - (const Float2& a) {
  return Float2(-a.x(), -a.y());
}

inline Float2
operator + (const Float2& a, const Float2& b) {
  return Float2(a.x() + b.x(), a.y() + b.y());
}

inline Float2
operator - (const Float2& a, const Float2& b) {
  return a + -b;
}

inline Float2
operator * (float fact, const Float2& a) {
  return Float2(fact*a.x(), fact*a.y());
}

inline Float2
operator * (const Float2& a, float fact) {
  return fact * a;
}

inline Float2
operator / (const Float2& a, float fact) {
  return a * (1.0f/fact);
}

inline Float2
normalize(const Float2& a) {
  float a_norm = norm(a);
  return Float2(a.x()/a_norm, a.y()/a_norm);
}

/**
 * Returns a rotation of vector `a` by the angle indicated
 * through the unit direction vector `cos_sin`.
 */
inline Float2
rotate(Float2 a, Float2 cos_sin) {
  // This effectively multiplies a rotation matrix based on cos_sin
  // to the vector a.
  return Float2(cos_sin.x()*a.x() - cos_sin.y()*a.y(),
                cos_sin.y()*a.x() + cos_sin.x()*a.y());
}

class Screen;

class Image {
  friend class Screen;

public:
  Image(Screen *screen, xSDL::Surface *surf);

  Image(Screen *screen, xSDL::Surface&& surf);

  Image(Image&& src)
    : tex(std::move(src.tex)), w(src.w), h(src.h)
  {}

  Image& operator = (Image&& src) {
    if (&src != this) {
      tex = std::move(src.tex);
      w = src.w;
      h = src.h;
    }
    return *this;
  }

  Image& operator = (const Image&) = delete;
  Image(const Image&) = delete;

  /**
   * Assumes the base of the image is at its mid-left point. This isn't a problem
   * if you're drawing your images with the appropriated Draw<DIR>Image call.
   */
  Float2
  center_for_base_at(Float2 base, Float2 unit_direction) {
    // Notice that unit_direction.x() is the cosine of the angle for
    // that direction. Also unit_direction.y() is the sine of the angle
    // for the direction.

    return base + 0.5f*height()*unit_direction;
  }

  /**
   * Assumes the base of the image is at its mid-left point. This isn't a problem
   * if you're drawing your images with the appropriated Draw<DIR>Image call.
   */
  Float2
  center_for_top_at(Float2 top, Float2 unit_direction) {
    // Notice that unit_direction.x is the cosine of the angle for
    // that direction. Also unit_direction.y is the sine of the angle
    // for the direction.

    return top - 0.5f*height()*unit_direction;
  }

  int
  width() const {
    return w;
  }

  int
  height() const {
    return h;
  }

private:
  xSDL::Texture tex;
  int w, h;
};

class Screen {
  friend class Image;
public:
  Screen(xSDL::Renderer *rend, int width, int height)
    : rend(rend), w(width), h(height)
  {}

  Screen(Screen&& src)
    : rend(std::move(src.rend)), w(src.w), h(src.h)
  {}

  Screen& operator = (Screen&& src) {
    if (&src != this) {
      rend = std::move(src.rend);
      w = src.w;
      h = src.h;
    }
    return *this;
  }

  Screen& operator = (const Screen&) = delete;
  Screen(const Screen&) = delete;

  Image
  load_image(const char *file_name) {
    return Image(this, xIMG::load(file_name));
  }

  void
  fill_square(Float2 center, float side, xSDL::Color color) {
    rend->set_draw_color(color);
    int side_i = static_cast<int>(side);
    SDL_Rect point = {static_cast<int>(center.x() - side/2.0f),
                      static_cast<int>(h - 1.0f - center.y() - side/2.0f),
                      side_i, side_i};
    rend->fill_rectangle(point);
  }

  /**
   * Draw the image with its CENTER at x,y and rotated by ANGLE around its
   * center. You should also specify x,y from left->right and bottom->up.
   *
   * @note The flip is as in SDL_RenderCopyEx.
   *
   * @note Angle is in radians.
   *
   * @note Generally speaking, the rotation is independent of the position,
   * which is independent of the flip. The parameters are chosen so that no
   * matter the order in which you think of applying these transformations
   * (translation, rotation, flip), you get the same results. This obviously
   * restricts what you can do, but it also makes it way simpler to use this
   * function.
   */
  void
  draw_image(Image *img, Float2 center,
             float angle,
             SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    constexpr double RAD_TO_DEG = 180.0 / M_PI;

    const float y = (h - 1.0f - center.y()) - img->height()/2.0f;
    const float x = center.x() - img->width()/2.0f;
    const SDL_Rect dest_rect = {static_cast<int>(x), static_cast<int>(y),
                                img->width(), img->height()};

    rend->copy(&img->tex, nullptr, &dest_rect, -angle*RAD_TO_DEG, nullptr, flip);;
  }

  void
  draw_image(Image *img, Float2 center,
             float angle, Float2 rot_center,
             SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    center -= rot_center;
    center = rotate(center, Float2(std::cos(angle), std::sin(angle)));
    center += rot_center;
    draw_image(img, center, angle, flip);
  }

  /**
   * This makes an image pointing "down" to be draw as itself rotated to
   * point "right".
   *
   * Basically, this is equivalent to calling:
   * draw_image(img, center, angle + M_PI*0.5f, flip);
   */
  void
  draw_image_90(Image *img, Float2 center,
                float angle = 0.0f,
                SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle + M_PI*0.5f, flip);
  }

  void
  draw_image_90(Image *img, Float2 center,
                float angle, Float2 rot_center,
                SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle + M_PI*0.5f, rot_center, flip);
  }

  /**
   * This makes an image pointing "left" to be draw as itself rotated to
   * point "right".
   *
   * Basically, this is equivalent to calling:
   * draw_image(img, x, y, angle + M_PI, flip);
   */
  void
  draw_image_180(Image *img, Float2 center,
                 float angle = 0.0f,
                 SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle + M_PI, flip);
  }

  void
  draw_image_180(Image *img, Float2 center,
                 float angle, Float2 rot_center,
                 SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle + M_PI, rot_center, flip);
  }

  /**
   * This makes an image pointing "up" to be drawn as itself rotated to
   * point "right".
   *
   * Basically, it's equivalent to calling:
   * draw_image(img, x, y, angle + M_PI*1.5f, flip).
   */
  void
  draw_image_270(Image *img, Float2 center,
                float angle = 0.0f,
                SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle - M_PI*0.5f, flip);
  }

  void
  draw_image_270(Image *img, Float2 center,
                 float angle, Float2 rot_center,
                 SDL_RendererFlip flip = SDL_FLIP_NONE)
  {
    draw_image(img, center, angle - M_PI*0.5f, rot_center, flip);
  }

  int
  width() const {
    return w;
  }

  int
  height() const {
    return h;
  }

private:
  xSDL::Renderer *rend;
  int w, h;
};

inline
Image::
Image(Screen *screen, xSDL::Surface *surf)
  : tex(screen->rend, surf), w(surf->width()), h(surf->height())
{}

inline
Image::
Image(Screen *screen, xSDL::Surface&& surf)
  : Image(screen, &surf)
{}

} // end of SHR

#endif
