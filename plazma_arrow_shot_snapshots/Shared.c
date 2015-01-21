#include <SDL.h>
#include <SDL_image.h>

#include "Shared.h"

/*
 * Several functions have their interfaces explained as a comment commented in
 * the header file. Check it out.
 */

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

  out_img->w = surf->w;
  out_img->h = surf->h;
  SDL_FreeSurface(surf);

  if (!tex) {
    return -1;
  }

  out_img->tex = tex;

  return 0;
}

void
SHR_DestroyImage(struct SHR_Image *img) {
  if (img && img->tex) {
    SDL_DestroyTexture(img->tex);
  }
}

int
SHR_DrawRightImage(struct SHR_Screen *screen,
                   struct SHR_Image *img,
                   int x, int y,
                   float angle,
                   SDL_RendererFlip flip)
{
  const double RAD_TO_DEG = 180.0 / M_PI;

  y = screen->h - 1 - y;
  angle *= -1;
  y -= img->h/2;
  x -= img->w/2;
  SDL_Rect dest_rect = {x, y, img->w, img->h};
  return SDL_RenderCopyEx(screen->rend, img->tex, NULL, &dest_rect,
                          angle*RAD_TO_DEG, NULL, flip);
}

int
SHR_DrawUpImage(struct SHR_Screen *screen,
                struct SHR_Image *img,
                int x, int y,
                float angle,
                SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, x, y, angle - M_PI*0.5, flip);
}

int
SHR_DrawLeftImage(struct SHR_Screen *screen,
                  struct SHR_Image *img,
                  int x, int y,
                  float angle,
                  SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, x, y, angle - M_PI, flip);
}

int
SHR_DrawDownImage(struct SHR_Screen *screen,
                  struct SHR_Image *img,
                  int x, int y,
                  float angle,
                  SDL_RendererFlip flip)
{
  return SHR_DrawRightImage(screen, img, x, y, angle - M_PI*1.5, flip);
}

struct SHR_Float2
SHR_CenterForBaseAt(const struct SHR_Image *img,
                    float base_x, float base_y,
                    float sin_angle, float cos_angle)
{
  float x = base_x + (img->h * cos_angle)/2.0f;
  float y = base_y + (img->h * sin_angle)/2.0f;

  return (struct SHR_Float2) {x, y};
}

struct SHR_Float2
SHR_CenterForTopAt(const struct SHR_Image *img,
                   float top_x, float top_y,
                   float sin_angle, float cos_angle)
{
  float x = top_x - (img->h * cos_angle)/2.0f;
  float y = top_y - (img->h * sin_angle)/2.0f;

  return (struct SHR_Float2) {x, y};
}

extern inline float
SHR_Norm_f2(struct SHR_Float2 float2);

extern inline float
SHR_Norm2_f2(struct SHR_Float2 float2);

extern inline float
SHR_Dot_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b);

extern inline struct SHR_Float2
SHR_Sub_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b);

extern inline struct SHR_Float2
SHR_Sub_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b);

extern inline struct SHR_Float2
SHR_Add_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b);

extern inline struct SHR_Float2
SHR_Scale_f2(float x, struct SHR_Float2 float2);

extern inline int
SHR_EpsEq(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b, float eps);
