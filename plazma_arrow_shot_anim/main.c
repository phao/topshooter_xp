#include <assert.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Shared.h"
#include "PlazmaArrowShot.h"

enum {
  SCREEN_WIDTH = 800,
  SCREEN_HEIGHT = 600,
  MAX_ANIMS = 20
};

static SDL_Window *win;
static struct SHR_Screen screen;
static struct SHR_Image halo_img, projectile_img, fire_img, splash_img;
static struct PAS_Anim anims[MAX_ANIMS];
static int which_anim;

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

  int load_status;

  load_status = SHR_LoadImage(screen.rend, "projectile.png", &projectile_img);
  projectile_img.w /= 2;
  projectile_img.h /= 2;
  ExitLt0(load_status);

  load_status = SHR_LoadImage(screen.rend, "fire.png", &fire_img);
  fire_img.w /= 2;
  fire_img.h /= 2;
  ExitLt0(load_status);

  load_status = SHR_LoadImage(screen.rend, "splash.png", &splash_img);
  splash_img.w /= 1.5f;
  splash_img.h /= 1.5f;
  ExitLt0(load_status);

  load_status = SHR_LoadImage(screen.rend, "halo.png", &halo_img);
  halo_img.w /= 2;
  halo_img.h /= 2;
  ExitLt0(load_status);

  for (int i = 0; i < MAX_ANIMS; i++) {
    PAS_AnimPutDefaultDurations(anims+i);
    PAS_AnimPutImages(anims+i, &halo_img, &fire_img, &projectile_img,
                      &splash_img);
    PAS_AnimStop(anims+i);
  }
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
RandomAnimStart(struct PAS_Anim *anim, Uint32 ms_now) {
  float angle;
  struct SHR_Float2 vel_ms, start_position;
  int rand_x, rand_y;

  angle = rand()/(float)RAND_MAX * M_PI*2.0f;
  vel_ms = SHR_Make_f2(cosf(angle), sinf(angle));
  SHR_ScaleInto_f2(&vel_ms, .5f);
  rand_x = rand() % SCREEN_WIDTH/5 + SCREEN_WIDTH/2 - SCREEN_WIDTH/10;
  rand_y = rand() % SCREEN_HEIGHT/5 + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/10;
  start_position = SHR_Make_f2(rand_x, rand_y);

  PAS_AnimStart(anim, start_position, vel_ms, angle, ms_now);
}

void
AdvanceAnim(struct PAS_Anim *anim, Uint32 ms_now) {
  if (PAS_AnimHasStarted(anim, ms_now)) {
    if (rand()%100 == 0 && PAS_AnimCanBeHit(anim, ms_now)) {
      PAS_AnimHit(anim, ms_now);
    }
    if (PAS_AnimDraw(&screen, anim, ms_now) == 0) {
      PAS_AnimStop(anim);
    }
  }
}

void
SpawnAnim(Uint32 ms_now) {
  if (!PAS_AnimIsSet(anims+which_anim)) {
    RandomAnimStart(anims+which_anim, ms_now);
    which_anim++;
    if (which_anim >= MAX_ANIMS) {
      which_anim = 0;
    }
  }
}

static void
UpdateAndRender(Uint32 ms_now) {
  DrawWholeScreenWebGrid(100, 100);

  SpawnAnim(ms_now);
  for (int i = 0; i < MAX_ANIMS; i++) {
    AdvanceAnim(anims+i, ms_now);
  }
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

    UpdateAndRender(end_frame_ms);
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
