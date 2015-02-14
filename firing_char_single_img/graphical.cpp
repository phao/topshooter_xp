#include <cmath>
#include <utility>
#include <stdexcept>

#include "xmath.hpp"
#include "xsdl.hpp"
#include "xsdl_image.hpp"
#include "graphical.hpp"

using xMATH::PI;
using xMATH::Float2;

namespace GRAL {

Image::
Image(Image&& src) noexcept
  : tex(std::move(src.tex)), w(src.w), h(src.h)
{}

Image::
Image(Screen *screen, xSDL::Surface *surf)
  : tex(screen->rend, surf), w(surf->width()), h(surf->height())
{}

Image::
Image(Screen *screen, xSDL::Surface&& surf)
  : Image(screen, &surf)
{}

Image&
Image::
operator=(Image&& src) noexcept {
  if (&src != this) {
    tex = std::move(src.tex);
    w = src.w;
    h = src.h;
  }
  return *this;
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>image call.
 */
Float2
Image::
center_for_base_at(Float2 base, Float2 unit_direction) noexcept {
  // Notice that unit_direction.x() is the cosine of the angle for
  // that direction. Also unit_direction.y() is the sine of the angle
  // for the direction.

  return base + 0.5f*height()*unit_direction;
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a
 * problem if you're drawing your images with the appropriated Draw<DIR>image
 * call.
 */
Float2
Image::
center_for_top_at(Float2 top, Float2 unit_direction) noexcept {
  // Notice that unit_direction.x is the cosine of the angle for
  // that direction. Also unit_direction.y is the sine of the angle
  // for the direction.

  return top - 0.5f*height()*unit_direction;
}

void
Image::
set_alpha_mod(uint8_t alpha_mod) {
  tex.set_alpha_mod(alpha_mod);
}

void
Image::
set_color_mod(xSDL::Color color_mod) {
  tex.set_color_mod(color_mod);
}

void
Image::
set_blend_mode(xSDL::BlendMode blend_mode) {
  tex.set_blend_mode(blend_mode);
}

uint8_t
Image::
get_alpha_mod() const {
  return tex.get_alpha_mod();
}

xSDL::Color
Image::
get_color_mod() const {
  return tex.get_color_mod();
}

xSDL::BlendMode
Image::
get_blend_mode() const {
  return tex.get_blend_mode();
}

int
Image::
width() const noexcept {
  return w;
}

int
Image::
height() const noexcept {
  return h;
}

Screen::Screen(xSDL::Renderer *rend, int width, int height) noexcept
  : rend(rend), w(width), h(height)
{}

Screen::Screen(Screen&& src) noexcept
  : rend(std::move(src.rend)), w(src.w), h(src.h)
{}

Screen&
Screen::
operator=(Screen&& src) noexcept {
  if (&src != this) {
    rend = std::move(src.rend);
    w = src.w;
    h = src.h;
  }
  return *this;
}

Image
Screen::
load_image(const char *file_name) {
  return Image(this, xIMG::load(file_name));
}

void
Screen::
fill_square(xMATH::Float2 center, float side, xSDL::Color color) {
  rend->set_draw_color(color);
  int side_i = static_cast<int>(side);
  xSDL::Rect rect = {static_cast<int>(center.x() - side/2.0f),
                     static_cast<int>(h - 1.0f - center.y() - side/2.0f),
                     side_i,
                     side_i};
  rend->fill_rectangle(rect);
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
Screen::
draw_image(Image *img, xMATH::Float2 center,
           float angle, xSDL::RenderFlip flip)
{
  constexpr double RAD_TO_DEG = 180.0 / PI<double>();

  const float y = (h - 1.0f - center.y()) - img->height()/2.0f;
  const float x = center.x() - img->width()/2.0f;
  const xSDL::Rect dest_rect = {static_cast<int>(x), static_cast<int>(y),
                                img->width(), img->height()};

  rend->copy(&img->tex, nullptr, &dest_rect, -angle*RAD_TO_DEG, nullptr,
             flip);
}

void
Screen::
draw_image(Image *img, xMATH::Float2 center,
           float angle, xMATH::Float2 rot_center,
           xSDL::RenderFlip flip)
{
  center -= rot_center;
  center = rotate(center, xMATH::Float2(std::cos(angle), std::sin(angle)));
  center += rot_center;
  draw_image(img, center, angle, flip);
}

/**
 * This makes an image pointing "down" to be draw as itself rotated to
 * point "right".
 *
 * Basically, this is equivalent to calling:
 * draw_image(img, center, angle + PI*0.5f, flip);
 */
void
Screen::
draw_image_90(Image *img, xMATH::Float2 center,
              float angle, xSDL::RenderFlip flip)
{
  draw_image(img, center, angle + PI<float>()*0.5f, flip);
}

void
Screen::
draw_image_90(Image *img, xMATH::Float2 center,
              float angle, xMATH::Float2 rot_center,
              xSDL::RenderFlip flip)
{
  draw_image(img, center, angle + PI<float>()*0.5f, rot_center,
             flip);
}

/**
 * This makes an image pointing "left" to be draw as itself rotated to
 * point "right".
 *
 * Basically, this is equivalent to calling:
 * draw_image(img, x, y, angle + PI, flip);
 */
void
Screen::
draw_image_180(Image *img, xMATH::Float2 center,
               float angle, xSDL::RenderFlip flip)
{
  draw_image(img, center, angle + PI<float>(), flip);
}

void
Screen::
draw_image_180(Image *img, xMATH::Float2 center,
               float angle, xMATH::Float2 rot_center,
               xSDL::RenderFlip flip)
{
  draw_image(img, center, angle + PI<float>(), rot_center,
             flip);
}

/**
 * This makes an image pointing "up" to be drawn as itself rotated to
 * point "right".
 *
 * Basically, it's equivalent to calling:
 * draw_image(img, x, y, angle + PI*1.5f, flip).
 */
void
Screen::
draw_image_270(Image *img, xMATH::Float2 center,
              float angle, xSDL::RenderFlip flip)
{
  draw_image(img, center, angle - PI<float>()*0.5f, flip);
}

void
Screen::
draw_image_270(Image *img, xMATH::Float2 center,
               float angle, xMATH::Float2 rot_center,
               xSDL::RenderFlip flip)
{
  draw_image(img, center, angle - PI<float>()*0.5f, rot_center,
             flip);
}

int
Screen::
width() const noexcept {
  return w;
}

int
Screen::
height() const noexcept {
  return h;
}


} // end of gral
