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

#include <SDL2/SDL.h>

#include "xMath.h"

struct GRAL_Image {
  SDL_Texture *tex;
  int w, h;
};

struct GRAL_Screen {
  SDL_Renderer *rend;
  int w, h;
};

typedef struct GRAL_Image GRAL_Image;

typedef struct GRAL_Screen GRAL_Screen;

/**
 * Initializes an image for a given screen from a given surface.
 */
int
GRAL_SetupImageFromSurface(GRAL_Screen *screen,
                           GRAL_Image *img,
                           SDL_Surface *surf);

/**
 * Initializes an image for a given screen from a given surface region. Notice
 * that region can't be null. If you need to setup an image from the whole
 * screen, use GRAL_SetupImageFromSurface.
 */
int
GRAL_SetupImageFromSurfaceRegion(GRAL_Screen *screen,
                                 GRAL_Image *img,
                                 SDL_Surface *src_surf,
                                 const SDL_Rect *region);

static inline void
GRAL_DestroyImage(GRAL_Image *img) {
  SDL_DestroyTexture(img->tex);
}

/**
 * Sets up a screen. You need to give a renderer and the size of the screen.
 * The width and height parameters (3rd and 4th) are supposed to be the size
 * of the whole screen the renderer renders to. It's an error not to do so.
 */
void
GRAL_SetupScreen(GRAL_Screen *screen, SDL_Renderer *rend,
                  int w, int h);

/**
 * Assumes the base of the image is at its mid-left point. This isn't a
 * problem if you're drawing your images with the appropriated
 * GRAL_DrawImage* call.
 */
static inline vec2f
GRAL_ImageCenterForBaseAt(GRAL_Image *img,
                          vec2f base,
                          vec2f unit_direction)
{
  // Notice that unit_direction.x() is the cosine of the angle for
  // that direction. Also unit_direction.y() is the sine of the angle
  // for the direction.
  return add_2f(base, mul_2f(unit_direction, 0.5f*img->h));
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a
 * problem if you're drawing your images with the appropriated
 * GRAL_DrawImage* call.
 */
static inline vec2f
GRAL_ImageCenterForTopAt(GRAL_Image *img,
                         vec2f top,
                         vec2f unit_direction)
{
  // Notice that unit_direction.x is the cosine of the angle for
  // that direction. Also unit_direction.y is the sine of the angle
  // for the direction.
  return sub_2f(top, mul_2f(unit_direction, 0.5f*img->h));
}

static inline int
GRAL_SetImageAlphaMod(GRAL_Image *img, uint8_t mod) {
  return SDL_SetTextureAlphaMod(img->tex, mod);
}

static inline int
GRAL_SetImageColorMod(GRAL_Image *img, SDL_Color mod) {
  return SDL_SetTextureColorMod(img->tex, mod.r, mod.g, mod.b);
}

static inline int
GRAL_SetImageBlendMode(GRAL_Image *img, SDL_BlendMode blend_mode) {
  return SDL_SetTextureBlendMode(img->tex, blend_mode);
}

static inline int
GRAL_ImageAlphaMod(GRAL_Image *img, Uint8 *mod) {
  return SDL_GetTextureAlphaMod(img->tex, mod);
}

static inline int
GRAL_ImageColorMod(GRAL_Image *img, SDL_Color *c) {
  return SDL_GetTextureColorMod(img->tex, &c->r, &c->g, &c->b);
}

static inline int
GRAL_ImageBlendMode(GRAL_Image *img, SDL_BlendMode *mode) {
  return SDL_GetTextureBlendMode(img->tex, mode);
}

static inline int
GRAL_ImageWidth(GRAL_Image *img) {
  return img->w;
}

static inline int
GRAL_ImageHeight(GRAL_Image *img) {
  return img->h;
}

int
GRAL_LoadImage(GRAL_Screen *screen,
               GRAL_Image *img,
               const char *file_name);

int
GRAL_FillSquare(GRAL_Screen *screen,
                vec2f center,
                float side,
                SDL_Color color);

/**
 * Draw the image with its CENTER at x,y and rotated by ANGLE around its
 * center. You should also specify x,y from left->right and bottom->up.
 *
 * @note Angle is in radians.
 */
int
GRAL_DrawImage(GRAL_Screen *screen,
               GRAL_Image *img,
               vec2f center,
               float angle);

int
GRAL_DrawImageRotRel(GRAL_Screen *screen,
                     GRAL_Image *img,
                     vec2f center,
                     float angle,
                     vec2f rot_center);

/**
 * This makes an image pointing "down" to be draw as itself rotated to
 * point "right".
 *
 * Basically, this is equivalent to calling:
 * GRAL_DrawImage(img, center, angle + PI*0.5f, flip);
 */
static inline int
GRAL_DrawImage90(GRAL_Screen *screen,
                 GRAL_Image *img,
                 vec2f center,
                 float angle)
{
  return GRAL_DrawImage(screen, img, center, angle + XM_PI_f*0.5f);
}

static inline int
GRAL_DrawImage90RotRel(GRAL_Screen *screen,
                       GRAL_Image *img,
                       vec2f center,
                       float angle,
                       vec2f rot_center)
{
  return GRAL_DrawImageRotRel(screen, img, center, angle + XM_PI_f*0.5f,
                              rot_center);
}

/**
 * This makes an image pointing "left" to be draw as itself rotated to
 * point "right".
 *
 * Basically, this is equivalent to calling:
 * GRAL_DrawImage(img, x, y, angle + PI, flip);
 */
static inline int
GRAL_DrawImage180(GRAL_Screen *screen,
                  GRAL_Image *img,
                  vec2f center,
                  float angle)
{
  return GRAL_DrawImage(screen, img, center, angle + XM_PI_f);
}

static inline int
GRAL_DrawImage180RotRel(GRAL_Screen *screen,
                        GRAL_Image *img,
                        vec2f center,
                        float angle,
                        vec2f rot_center)
{
  return GRAL_DrawImageRotRel(screen, img, center, angle + XM_PI_f,
                              rot_center);
}

/**
 * This makes an image pointing "up" to be drawn as itself rotated to
 * point "right".
 *
 * Basically, it's equivalent to calling:
 * GRAL_DrawImage(img, x, y, angle + PI*1.5f, flip).
 */
static inline int
GRAL_DrawImage270(GRAL_Screen *screen,
                  GRAL_Image *img,
                  vec2f center,
                  float angle)
{
  return GRAL_DrawImage(screen, img, center, angle - XM_PI_f*0.5f);
}

static inline int
GRAL_DrawImage270RotRel(GRAL_Screen *screen,
                        GRAL_Image *img,
                        vec2f center,
                        float angle,
                        vec2f rot_center)
{
  return GRAL_DrawImageRotRel(screen, img, center, angle - XM_PI_f*0.5f,
                              rot_center);
}

static inline int
GRAL_ScreenWidth(GRAL_Screen *screen) {
  return screen->w;
}

static inline int
GRAL_ScreenHeight(GRAL_Screen *screen) {
  return screen->h;
}

#endif
