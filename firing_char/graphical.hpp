#ifndef GRAPHICAL_H
#define GRAPHICAL_H

/**
 * As of now, this module contains basic type definitions, also drawing and
 * positioning functionalities. Some important assumptions are also baked into
 * here:
 *
 * - Things should be in float values as much as possible.
 *
 * - Drawing an image involves specifying a rotation around its center, and
 * the position of its center in the destination screen (it also involves
 * specifying more things, but these 2 are the most important).
 *
 * - Consider using center_for_<base|top>_at functions for calculating
 * positions of where to draw images.
 *
 * - draw_image_<ROT_ANGLE> assumes the following coordinate system:
 * bottom-left screen point is the origin; y grows up; x grows right.
 *
 * - If your image doesn't point right, you should manipulate it as if it
 * did and then call the right drawing function when drawing (e.g. if your
 * image points down, you should call DrawDownimage). If you have an image
 * which naturally points at 55 degrees, what you're going to do is treat it
 * as if it pointed right and you'll draw it with something like
 * draw_image(..., -55 degrees in radians, ...). Although this isn't strictly
 * required, it makes things easier.
 *
 * Treating images this way can be quite confusing if you're doing all the
 * coordinates transformations yourself, so you should try to as much as
 * possible think in terms of draw_image_<ROT> and center_for_<base|top>_at.
 * While doing *your own* manipulations, if you stick to the assumption that
 * images point right and so forth, things actually become very
 * straightforward. The complication comes in if you try to bypass
 * draw_image_<ROT> and/or center_for_<base|top>_at.
 *
 * Once you know the rotation (around its center) the image should have and
 * its base (or top) point, you should call center_for_<base|top>_at to get
 * the values for the center point. Then you should call draw_image_<ROT> for
 * drawing.
 *
 * center_for_<base|top>_at is basically your positioning routine. With the
 * given scheme for dealing with images, it's quite difficult to figure out
 * by head or even on paper where everything should go. Thinking in terms of
 * (1) base points for images, (2) top points for images, (3) images pointing
 * right, and (4) coordinate systems where y-positive is up makes things
 * extremely easier. This happens because, then, given you draw your image
 * correctly (i.e. with the right draw_image_<ROT> call), all you need to
 * know are things like:
 *
 * - What is the center point (which draw_image_<ROT> takes) so that the base
 * or top point is <such> for a center rotation of <so much>?
 *
 * This question is basically what center_for_<base|top>_at answers. It lets
 * you give it the base or top point you'd like to have, the rotation you'd
 * like to have for the image, and it spills you out the center point you
 * should pass to draw_image_<ROT> together with the rotation value so the
 * image gets drawn as you'd expect.
 */

#include <cmath>
#include <utility>
#include <stdexcept>

#include "xmath.hpp"
#include "xsdl.hpp"
#include "xsdl_image.hpp"

namespace gral {

class Screen;

class Image {
  friend class Screen;

public:
  Image(Screen *Screen, xsdl::Surface *surf);
  Image(Screen *Screen, xsdl::Surface&& surf);

  Image(Image&& src) noexcept;
  Image& operator=(Image&& src) noexcept;

  Image& operator=(const Image&) = delete;
  Image(const Image&) = delete;

  /**
   * Assumes the base of the image is at its mid-left point. This isn't a
   * problem if you're drawing your images with the appropriated
   * draw_image_<ROT> call.
   */
  xmath::Float2
  center_for_base_at(xmath::Float2 base,
                     xmath::Float2 unit_direction) noexcept;

  /**
   * Assumes the base of the image is at its mid-left point. This isn't a
   * problem if you're drawing your images with the appropriated
   * draw_image_<ROT> call.
   */
  xmath::Float2
  center_for_top_at(xmath::Float2 top,
                    xmath::Float2 unit_direction) noexcept;

  void
  set_alpha_mod(uint8_t alpha_mod);

  void
  set_color_mod(xsdl::Color color_mod);

  void
  set_blend_mode(xsdl::BlendMode blend_mode);

  uint8_t
  get_alpha_mod() const;

  xsdl::Color
  get_color_mod() const;

  xsdl::BlendMode
  get_blend_mode() const;

  int
  width() const noexcept;

  int
  height() const noexcept;

private:
  xsdl::Texture tex;
  int w, h;
};

class BlendModeGuard {
public:
  BlendModeGuard(Image *img, xsdl::BlendMode blend_mode)
    : img {img}, restore_blend_mode {img->get_blend_mode()}
  {
    img->set_blend_mode(blend_mode);
  }

  ~BlendModeGuard() {
    try {
      img->set_blend_mode(restore_blend_mode);
    }
    catch (...) {
      // We do nothing if this fail.
    }
  }

private:
  Image *img;
  xsdl::BlendMode restore_blend_mode;
};

class ColorModGuard {
public:
  ColorModGuard(Image *img, xsdl::Color color)
    : img {img}, restore_color {img->get_color_mod()}
  {
    img->set_color_mod(color);
  }

  ~ColorModGuard() {
    try {
      img->set_color_mod(restore_color);
    }
    catch (...) {
      // We do nothing if this fail.
    }
  }

private:
  Image *img;
  xsdl::Color restore_color;
};

class AlphaModGuard {
public:
  AlphaModGuard(Image *img, uint8_t alpha_mod)
    : img {img}, restore_alpha {img->get_alpha_mod()}
  {
    img->set_alpha_mod(alpha_mod);
  }

  ~AlphaModGuard() {
    try {
      img->set_alpha_mod(restore_alpha);
    }
    catch (...) {
      // We do nothing if this fail.
    }
  }

private:
  Image *img;
  uint8_t restore_alpha;
};

class Screen {
  friend class Image;

public:
  Screen(xsdl::Renderer *rend, int width, int height) noexcept;

  Screen(Screen&& src) noexcept;
  Screen& operator=(Screen&& src) noexcept;

  Screen& operator=(const Screen&) = delete;
  Screen(const Screen&) = delete;

  Image
  load_image(const char *file_name);

  void
  fill_square(xmath::Float2 center, float side, xsdl::Color color);

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
  draw_image(Image *img, xmath::Float2 center,
             float angle,
             SDL_RendererFlip flip = SDL_FLIP_NONE);

  void
  draw_image(Image *img, xmath::Float2 center,
             float angle, xmath::Float2 rot_center,
             SDL_RendererFlip flip = SDL_FLIP_NONE);

  /**
   * This makes an image pointing "down" to be draw as itself rotated to
   * point "right".
   *
   * Basically, this is equivalent to calling:
   * draw_image(img, center, angle + PI*0.5f, flip);
   */
  void
  draw_image_90(Image *img, xmath::Float2 center,
                float angle = 0.0f,
                SDL_RendererFlip flip = SDL_FLIP_NONE);

  void
  draw_image_90(Image *img, xmath::Float2 center,
                float angle, xmath::Float2 rot_center,
                SDL_RendererFlip flip = SDL_FLIP_NONE);

  /**
   * This makes an image pointing "left" to be draw as itself rotated to
   * point "right".
   *
   * Basically, this is equivalent to calling:
   * draw_image(img, x, y, angle + PI, flip);
   */
  void
  draw_image_180(Image *img, xmath::Float2 center,
                 float angle = 0.0f,
                 SDL_RendererFlip flip = SDL_FLIP_NONE);

  void
  draw_image_180(Image *img, xmath::Float2 center,
                 float angle, xmath::Float2 rot_center,
                 SDL_RendererFlip flip = SDL_FLIP_NONE);

  /**
   * This makes an image pointing "up" to be drawn as itself rotated to
   * point "right".
   *
   * Basically, it's equivalent to calling:
   * draw_image(img, x, y, angle + PI*1.5f, flip).
   */
  void
  draw_image_270(Image *img, xmath::Float2 center,
                 float angle = 0.0f,
                 SDL_RendererFlip flip = SDL_FLIP_NONE);

  void
  draw_image_270(Image *img, xmath::Float2 center,
                 float angle, xmath::Float2 rot_center,
                 SDL_RendererFlip flip = SDL_FLIP_NONE);

  int
  width() const noexcept;

  int
  height() const noexcept;

private:
  xsdl::Renderer *rend;
  int w, h;
};

} // end of gral

#endif
