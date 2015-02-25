#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "xMath.h"
#include "Graphical.h"

int
GRAL_SetupImageFromSurface(GRAL_Screen *screen,
                           GRAL_Image *img,
                           SDL_Surface *surf)
{
  img->w = surf->w;
  img->h = surf->h;
  img->tex = SDL_CreateTextureFromSurface(screen->rend, surf);
  return img->tex ? 0 : -1;
}

int
GRAL_SetupImageFromSurfaceRegion(GRAL_Screen *screen,
                                 GRAL_Image *img,
                                 SDL_Surface *src_surf,
                                 const SDL_Rect *region)
{
  SDL_PixelFormat *f = src_surf->format;
  int w = region->w;
  int h = region->h;
  SDL_Surface *aux = SDL_CreateRGBSurface(0, w, h, f->BitsPerPixel, f->Rmask,
                                          f->Gmask, f->Bmask, f->Amask);
  if (!aux) {
    return -1;
  }
  if (SDL_BlitSurface(src_surf, region, aux, NULL) < 0) {
    SDL_FreeSurface(aux);
    return -1;
  }
  img->w = w;
  img->h = h;
  img->tex = SDL_CreateTextureFromSurface(screen->rend, aux);
  SDL_FreeSurface(aux);
  return img->tex ? 0 : -1;
}

void
GRAL_SetupScreen(GRAL_Screen *screen, SDL_Renderer *rend,
                  int w, int h)
{
  screen->rend = rend;
  screen->w = w;
  screen->h = h;
}

int
GRAL_LoadImage(GRAL_Screen *screen,
               GRAL_Image *img,
               const char *file_name)
{
  SDL_Surface *surf = IMG_Load(file_name);
  if (!surf) {
    return -1;
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(screen->rend, surf);
  int w = surf->w;
  int h = surf->h;
  SDL_FreeSurface(surf);
  if (!tex) {
    return -2;
  }
  img->w = w;
  img->h = h;
  img->tex = tex;
  return 0;
}

int
GRAL_FillSquare(GRAL_Screen *screen,
                vec2f center,
                float side,
                SDL_Color color)
{
  int status = SDL_SetRenderDrawColor(screen->rend, color.r, color.g,
                                      color.b, color.a);
  if (status < 0) {
    return status;
  }
  float half_side = side*0.5f;
  SDL_Rect rect = {
    .x = center.x - half_side,
    .y = screen->h - 1.0f - center.y - half_side,
    .w = side,
    .h = side
  };
  return SDL_RenderFillRect(screen->rend, &rect);
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
GRAL_DrawImage(GRAL_Screen *screen,
               GRAL_Image *img,
               vec2f center,
               float angle)
{
  double RAD_TO_DEG = 180.0 / XM_PI_d;
  float y = (screen->h - 1.0f - center.y) - img->h*0.5f;
  float x = center.x - img->w*0.5f;
  const SDL_Rect dest_rect = {x, y, img->w, img->h};

  return SDL_RenderCopyEx(screen->rend, img->tex,
                          NULL, &dest_rect,
                          -angle*RAD_TO_DEG, NULL,
                          SDL_FLIP_NONE);
}

int
GRAL_DrawImageRotRel(GRAL_Screen *screen,
                     GRAL_Image *img,
                     vec2f center,
                     float angle,
                     vec2f rot_center)
{
  subto_2f(&center, rot_center);
  rotto_2f(&center, (vec2f){cosf(angle), sinf(angle)});
  addto_2f(&center, rot_center);
  return GRAL_DrawImage(screen, img, center, angle);
}
