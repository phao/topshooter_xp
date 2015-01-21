#include <assert.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Shared.h"

enum {
  SCREEN_WIDTH = 800,
  SCREEN_HEIGHT = 600,
  SCREEN_CENTER_X = SCREEN_WIDTH/2,
  SCREEN_CENTER_Y = SCREEN_HEIGHT/2,
};

static SDL_Window *win;
static struct SHR_Screen screen;
static struct SHR_Image projectile_img, fire_img, splash_img;

static void
Cleanup(void) {
  SHR_DestroyImage(&projectile_img);
  if (screen.rend) {
    SDL_DestroyRenderer(screen.rend);
  }
  if (win) {
    SDL_DestroyWindow(win);
  }
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

static void
Init(void) {
  ExitLt0(SDL_Init(SDL_INIT_VIDEO));

  win = SDL_CreateWindow("Simple Shot", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN);

  ExitIf0(win);

  int render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  screen.rend = SDL_CreateRenderer(win, -1, render_flags);

  ExitIf0(screen.rend);

  screen.h = SCREEN_HEIGHT;
  screen.w = SCREEN_WIDTH;

  int img_load_flags = IMG_INIT_PNG;
  int mask_loaded = IMG_Init(img_load_flags) & img_load_flags;

  ExitIf0(mask_loaded);

  int load_status = SHR_LoadImage(screen.rend, "projectile.png",
                                  &projectile_img);
  projectile_img.w /= 2;
  projectile_img.h /= 2;
  ExitLt0(load_status);

  load_status = SHR_LoadImage(screen.rend, "fire.png", &fire_img);
  fire_img.w /= 2;
  fire_img.h /= 2;
  ExitLt0(load_status);

  load_status = SHR_LoadImage(screen.rend, "splash.png", &splash_img);
  splash_img.w /= 2;
  splash_img.h /= 2;
  ExitLt0(load_status);
}

static void
DrawWholeScreenWebGrid(int cell_w, int cell_h) {
  SDL_SetRenderDrawColor(screen.rend, 128, 128, 128, 255);

  for (int x = 0; x < SCREEN_WIDTH; x += cell_w) {
    SDL_RenderDrawLine(screen.rend, x, 0, x, SCREEN_HEIGHT-1);
  }
  for (int y = 0; y < SCREEN_HEIGHT; y += cell_h) {
    SDL_RenderDrawLine(screen.rend, 0, y, SCREEN_WIDTH-1, y);
  }

  for (int x = 0; x < SCREEN_WIDTH; x += cell_w) {
    for (int y = 0; y < SCREEN_HEIGHT; y += cell_h) {
      SDL_RenderDrawLine(screen.rend, x, y, x+cell_w, y+cell_h);
      SDL_RenderDrawLine(screen.rend, x+cell_w, y, x, y+cell_h);
    }
  }
}

static void
DrawShotPath(struct SHR_Float2 shot_point,
             struct SHR_Float2 splash_point)
{
  struct SHR_Float2 vec = SHR_Sub_f2(splash_point, shot_point);
  float vec_length = SHR_Norm_f2(vec);

  // These assertions are mostly to catch mistakes of mine if I screw up
  // vector arithmetic.
  //
  // Note: things are considered equal here if their distances is less than
  // a half a pixel's width.

  assert(SHR_EpsEq(splash_point, SHR_Add_f2(shot_point, vec), 0.5f));
  assert(vec_length >= 0);

  // Should I have something like the inverse of vec_length calculated
  // so I avoid this division?

  float sin_angle = vec.y/vec_length;
  float cos_angle = vec.x/vec_length;
  float angle = atan2f(sin_angle, cos_angle);

  struct SHR_Float2 center;

  center = SHR_CenterForBaseAt(&fire_img,
                               shot_point.x, shot_point.y,
                               sin_angle, cos_angle);

  SHR_DrawDownImage(&screen, &fire_img, center.x, center.y, angle, 0);
  SHR_DrawUpImage(&screen, &splash_img,
                  splash_point.x, splash_point.y, angle, 0);

  float projectile_path_length = vec_length - projectile_img.h;
  float path_t = 0;

  int snapshots_count = vec_length / 100;

  struct SHR_Float2 unit_vec = {cos_angle, sin_angle};

  for (int i = 0; i <= snapshots_count-1; i++) {
    struct SHR_Float2 projectile_point;
    struct SHR_Float2 diff;

    diff = SHR_Scale_f2(path_t * projectile_path_length, unit_vec);
    projectile_point = SHR_Add_f2(shot_point, diff);

    center = SHR_CenterForBaseAt(&projectile_img,
                                 projectile_point.x, projectile_point.y,
                                 sin_angle, cos_angle);

    SHR_DrawUpImage(&screen, &projectile_img,
                    center.x, center.y,
                    angle, 0);

    path_t += 1.0f/(snapshots_count-1);
  }
}

static void
Draw(Uint32 at_ms) {
  (void) at_ms;

  DrawWholeScreenWebGrid(50, 50);
  DrawShotPath((struct SHR_Float2){450, 450},
               (struct SHR_Float2){100, 50});
  DrawShotPath((struct SHR_Float2){50, 550},
               (struct SHR_Float2){700, 500});
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

    int status = SDL_SetRenderDrawColor(screen.rend, 16, 16, 16, 255);
    ExitLt0(status);

    status = SDL_RenderClear(screen.rend);
    ExitLt0(status);

    Draw(end_frame_ms);
    SDL_RenderPresent(screen.rend);
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
