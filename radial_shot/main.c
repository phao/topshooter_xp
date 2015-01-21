#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Image {
  SDL_Texture *tex;
  int w, h;
};

enum {
  SCREEN_WIDTH = 800,
  SCREEN_HEIGHT = 600,
  SCREEN_CENTER_X = SCREEN_WIDTH/2,
  SCREEN_CENTER_Y = SCREEN_HEIGHT/2,
};

static SDL_Window *win;
static SDL_Renderer *rend;
static struct Image projectile_img;

static void
Cleanup(void) {
  if (rend) SDL_DestroyRenderer(rend);
  if (win) SDL_DestroyWindow(win);
  IMG_Quit();
  SDL_Quit();
}

static void
ErrorExit(void) {
  fputs("Error!\n", stderr);
  fputs("Libc error: ", stderr);
  fputs(strerror(errno), stderr);
  fputs("\nSDL error: ", stderr);
  fputs(SDL_GetError(), stderr);
  fputs("\nSDL_image error: ", stderr);
  fputs(IMG_GetError(), stderr);
  fputc('\n', stderr);
  Cleanup();
  exit(EXIT_FAILURE);
}

#define ExitIf0(v) if ((v) == 0) ErrorExit(); else (void) 0
#define ExitLt0(v) if ((v) < 0) ErrorExit(); else (void) 0

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Float2 {
  float x, y;
};

static struct Image
LoadImage(const char *file_name) {
  SDL_Surface *surf = IMG_Load(file_name);

  ExitIf0(surf);

  SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surf);

  struct Image img;
  img.w = surf->w;
  img.h = surf->h;

  SDL_FreeSurface(surf);

  ExitIf0(tex);

  img.tex = tex;

  return img;
}

static void
Init(void) {
  ExitLt0(SDL_Init(SDL_INIT_VIDEO));

  win = SDL_CreateWindow("Simple Shot", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN);

  ExitIf0(win);

  int render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win, -1, render_flags);

  ExitIf0(rend);

  int img_load_flags = IMG_INIT_PNG;
  int mask_loaded = IMG_Init(img_load_flags) & img_load_flags;

  ExitIf0(mask_loaded);

  projectile_img = LoadImage("projectile.png");

}

static void
DrawWholeScreenGrid(int w, int h) {
  SDL_SetRenderDrawColor(rend, 128, 128, 128, 255);

  for (int x = 0; x < SCREEN_WIDTH; x += w) {
    SDL_RenderDrawLine(rend, x, 0, x, SCREEN_HEIGHT-1);
  }
  for (int y = 0; y < SCREEN_HEIGHT; y += h) {
    SDL_RenderDrawLine(rend, 0, y, SCREEN_WIDTH-1, y);
  }
}

/**
 * Draw the image with its CENTER at x,y and rotated by ANGLE around its
 * center. You should also specify x,y from left->right and bottom->up.
 *
 * @note The flip is as in SDL_RenderCopyEx.
 *
 * @note Angle is in radians.
 *
 * @note Generally speaking, the rotation is independent of the position, which
 * is independent of the flip. The parameters are chosen so that no matter
 * the order in which you think of applying these transformations (translation,
 * rotation, flip), you get the same results. This obviously restricts what
 * you can do, but it also makes it way simpler to use this function.
 */
static void
DrawLeftImage(struct Image *img,
              int x, int y,
              float angle,
              SDL_RendererFlip flip)
{
  const double RAD_TO_DEG = 180.0 / M_PI;

  y = SCREEN_HEIGHT - 1 - y;
  angle *= -1;
  y -= img->h/2;
  x -= img->w/2;
  SDL_Rect dest_rect = {x, y, img->w, img->h};
  int status = SDL_RenderCopyEx(rend, img->tex, NULL, &dest_rect,
                                angle*RAD_TO_DEG, NULL, flip);
  ExitLt0(status);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points up (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * DrawLeftImage(img, x, y, angle - M_PI*0.5, flip)
 */
static void
DrawUpImage(struct Image *img,
            int x, int y,
            float angle,
            SDL_RendererFlip flip)
{
  DrawLeftImage(img, x, y, angle - M_PI*0.5, flip);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points right (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * DrawLeftImage(img, x, y, angle - M_PI, flip)
 */
static void
DrawRightImage(struct Image *img,
               int x, int y,
               float angle,
               SDL_RendererFlip flip)
{
  DrawLeftImage(img, x, y, angle - M_PI, flip);
}

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points down (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * DrawLeftImage(img, x, y, angle - M_PI*1.5, flip)
 */
static void
DrawBottomImage(struct Image *img,
                int x, int y,
                float angle,
                SDL_RendererFlip flip)
{
  DrawLeftImage(img, x, y, angle - M_PI*1.5, flip);
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
static struct Float2
CenterForBaseAt(const struct Image *img,
                float base_x, float base_y,
                float sin_angle, float cos_angle)
{
  float x = base_x + (img->h * cos_angle)/2.0f;
  float y = base_y + (img->h * sin_angle)/2.0f;

  return (struct Float2) {x, y};
}

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
static struct Float2
CenterForTopAt(const struct Image *img,
                float top_x, float top_y,
                float sin_angle, float cos_angle)
{
  float x = top_x - (img->h * cos_angle)/2.0f;
  float y = top_y - (img->h * sin_angle)/2.0f;

  return (struct Float2) {x, y};
}

static void
Animate(Uint32 at_ms) {
  enum {
    N_SHOTS = 32
  };

  DrawWholeScreenGrid(100, 100);

  SDL_SetTextureBlendMode(projectile_img.tex, SDL_BLENDMODE_ADD);

  const int rad_max = 350;

  float rad = at_ms/5 % rad_max;
  float t = rad/rad_max;

  float base_x = SCREEN_WIDTH/2.0f;
  float base_y = SCREEN_HEIGHT/2.0f;

  for (int i = 0; i < N_SHOTS; i++) {
    float angle = i * M_PI*2.0/N_SHOTS;
    float sin_angle = sinf(angle);
    float cos_angle = cosf(angle);
    struct Float2 center = CenterForTopAt(&projectile_img,
                                          base_x + cos_angle*rad,
                                          base_y + sin_angle*rad,
                                          sin_angle, cos_angle);
    SDL_SetTextureAlphaMod(projectile_img.tex, 255.0 * (1 - t));
    DrawUpImage(&projectile_img, center.x + 0.5f, center.y + 0.5f,
                angle, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(projectile_img.tex, 255.0 * t);
    DrawUpImage(&projectile_img, center.x + 0.5f, center.y + 0.5f,
                angle, SDL_FLIP_HORIZONTAL);
  }

  /*
  DrawWholeScreenGrid(100, 100);
  float angle = M_PI/4;
  float sin_angle = sin(angle);
  float cos_angle = cos(angle);
  SDL_Point center = CenterForBaseAt(&projectile_img,
                                     100,
                                     100,
                                     sin_angle, cos_angle);
  fprintf(stderr, "%d, %d\n", center.x, center.y);
  DrawUpImage(&projectile_img, center.x, center.y, angle, SDL_FLIP_NONE);
  */
}

static void
AnimLoop(void) {
  SDL_Event e;

  for (;;) {
    Uint32 end_frame_ms = SDL_GetTicks() + 16;

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }

    int status = SDL_SetRenderDrawColor(rend, 16, 16, 16, 255);
    ExitLt0(status);

    status = SDL_RenderClear(rend);
    ExitLt0(status);

    Animate(end_frame_ms);
    SDL_RenderPresent(rend);
  }
}

int
main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  Init();
  AnimLoop();
  Cleanup();

  return 0;
}
