#include <cassert>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#include <SDL.h>
#include <SDL_image.h>

#include "Shared.hpp"
#include "PlazmaArrowShot.hpp"

using namespace std:
using SHR::Float2;

enum {
  SCREEN_WIDTH = 800,
  SCREEN_HEIGHT = 600
};

class Game {
  public:
    Game(SDL_Window *win,
         SHR::Screen &&screen,
         SHR::Image &&halo_img,
         SHR::Image &&projectile_img,
         SHR::Image &&fire_img,
         SHR::Image &&splash_img)
      : win(win),
        screen(screen),
        hallo_img(hallo_img),
        projectile_img(projectile_img),
        fire_img(fire_img),
        splash_img(splash_img)
    {
    }

    void
    Loop() {
    }

    ~Game() {
      if (win) {
        SDL_DestroyWindow(win);
      }
      IMG_Quit();
      SDL_Quit();
    }

  private:
    SDL_Window *win;
    SHR::Screen screen;
    SHR::Image halo_img, projectile_img, fire_img, splash_img;
    PAS::Animation anims[2];
    int which_anim;
};

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
  splash_img.w /= 2;
  splash_img.h /= 2;
  ExitLt0(load_status);

  PAS_AnimPutDefaultDurations(anims+0);
  PAS_AnimPutImages(anims+0, &halo_img, &fire_img, &projectile_img,
                    &splash_img);
  PAS_AnimKill(anims+0);

  PAS_AnimPutDefaultDurations(anims+1);
  PAS_AnimPutImages(anims+1, &halo_img, &fire_img, &projectile_img,
                    &splash_img);
  PAS_AnimKill(anims+1);
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
RandomAnimStart(struct PAS_Anim *anim, Uint32 at_ms) {
  float angle;
  struct SHR_Float2 vel_ms, start_position;

  angle = rand()/(float)RAND_MAX * M_PI*2.0f;
  vel_ms = SHR_Make_f2(cosf(angle), sinf(angle));
  start_position = SHR_Make_f2(rand() % SCREEN_WIDTH,
                               rand() % SCREEN_HEIGHT);

  PAS_AnimStart(anim, start_position, vel_ms, angle, at_ms);
}

static void
Draw(Uint32 at_ms) {

  at_ms /= 10;

  DrawWholeScreenWebGrid(100, 100);

  if (rand()%10 == 0) {
    PAS_AnimHit(anims+0, at_ms);
  }
  PAS_AnimDraw(&screen, anims+0, at_ms);
  if (PAS_AnimStage(anims+0, at_ms) > 0) {
    PAS_AnimKill(anims+0);
  }

  if (rand()%10 == 0) {
    PAS_AnimHit(anims+1, at_ms);
  }
  PAS_AnimDraw(&screen, anims+1, at_ms);
  if (PAS_AnimStage(anims+1, at_ms) != 0) {
    PAS_AnimKill(anims+1);
  }
}

static void
ConsumeEvent(SDL_Event *e, Uint32 at_ms) {
  at_ms /= 10;

  if (e->type == SDL_KEYUP && e->key.keysym.sym == SDLK_SPACE) {
    RandomAnimStart(anims+which_anim, at_ms);
    which_anim++;
    if (which_anim > 1) {
      which_anim = 0;
    }
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
      ConsumeEvent(&e, end_frame_ms);
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
