#include <SDL.h>
#include <SDL_image.h>

#include "Shared.h"

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
struct SHR_Float2
SHR_CenterForBaseAt(const struct SHR_Image *img,
                    struct SHR_Float2 base,
                    struct SHR_Float2 unit_direction)
{
  // Notice that unit_direction.x is the cosine of the angle for
  // that direction. Also unit_direction.y is the sine of the angle
  // for the direction.

  float x = base.x + (img->h * unit_direction.x)/2.0f;
  float y = base.y + (img->h * unit_direction.y)/2.0f;

  return (struct SHR_Float2){x, y};
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
struct SHR_Float2
SHR_CenterForTopAt(const struct SHR_Image *img,
                   struct SHR_Float2 top,
                   struct SHR_Float2 unit_direction)
{
  // Notice that unit_direction.x is the cosine of the angle for
  // that direction. Also unit_direction.y is the sine of the angle
  // for the direction.

  float x = top.x - (img->h * unit_direction.x)/2.0f;
  float y = top.y - (img->h * unit_direction.y)/2.0f;

  return (struct SHR_Float2){x, y};
}

void
SHR_DestroyImage(struct SHR_Image *img) {
  if (img && img->tex) {
    SDL_DestroyTexture(img->tex);
  }
}

int
SHR_LoadImage(SDL_Renderer *rend,
              const char *file_name,
              struct SHR_Image *out_img)
{
  SDL_Surface *surf = IMG_Load(file_name);

  if (!surf) {
    return -1;
  }

  SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surf);

  out_img->tex = tex;
  out_img->w = surf->w;
  out_img->h = surf->h;

  SDL_FreeSurface(surf);

  if (!tex) {
    return -1;
  }

  return 0;
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
int
SHR_DrawRightImage(struct SHR_Screen *screen,
                   const struct SHR_Image *img,
                   struct SHR_Float2 center,
                   float angle,
                   SDL_RendererFlip flip)
{
  const double RAD_TO_DEG = 180.0 / M_PI;

  float x, y;

  y = (screen->h - 1 - center.y) - img->h/2;
  x = center.x - img->w/2;
  angle *= -1;
  SDL_Rect dest_rect = {x, y, img->w, img->h};
  return SDL_RenderCopyEx(screen->rend, img->tex, NULL, &dest_rect,
                          angle*RAD_TO_DEG, NULL, flip);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to
 * correct for the fact that the image points up (and not left) by
 * default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI*0.5, flip)
 */
int
SHR_DrawUpImage(struct SHR_Screen *screen,
                const struct SHR_Image *img,
                struct SHR_Float2 center,
                float angle,
                SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, center, angle - M_PI*0.5, flip);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to
 * correct for the fact that the image points right (and not left) by
 * default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI, flip)
 */
int
SHR_DrawLeftImage(struct SHR_Screen *screen,
                  const struct SHR_Image *img,
                  struct SHR_Float2 center,
                  float angle,
                  SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, center, angle - M_PI, flip);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to
 * correct for the fact that the image points down (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI*1.5, flip)
 */
int
SHR_DrawDownImage(struct SHR_Screen *screen,
                  const struct SHR_Image *img,
                  struct SHR_Float2 center,
                  float angle,
                  SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, center, angle - M_PI*1.5, flip);
}

