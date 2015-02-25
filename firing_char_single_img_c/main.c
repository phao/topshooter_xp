#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Graphical.h"
#include "Engineer.h"
#include "Particles.h"
#include "Atlas.h"

static GRAL_Screen screen;
static SDL_Surface *atlas;
static SDL_Window *win;
static SDL_Renderer *rend;
static GRAL_Image eng_skeleton[ENG_NUM_BODY_PIECES];
static GRAL_Image fire_particle;
static ENG_Character player;
static PARTS parts;

#define Err() \
  do { \
    Cleanup(); \
    fprintf(stderr, "Error: %s: %s: L(%d).\n" \
                    "SDL_GetError() => %s.\n" \
                    "IMG_GetError() => %s.\n" \
                    "strerror(errno) => %s.\n", \
                    __FILE__, __func__, __LINE__, \
                    SDL_GetError(), IMG_GetError(), \
                    strerror(errno)); \
    exit(EXIT_FAILURE); \
  } while (0)

#define ErrLt0(val) \
  do { \
    if ((val) < 0) { \
      Err(); \
    } \
  } while (0)

#define ErrIf0(val) \
  do { \
    if (!(val)) { \
      Err(); \
    } \
  } while (0)

static void
Cleanup(void) {
  if (atlas) {
    SDL_FreeSurface(atlas);
  }
  if (rend) {
    SDL_DestroyRenderer(rend);
  }
  if (win) {
    SDL_DestroyWindow(win);
  }
  IMG_Quit();
  SDL_Quit();
}

static void
EngSkeletonSetup(void) {
  for (int i = 0; i < ENG_NUM_BODY_PIECES; ++i) {
    ErrLt0(ATLAS_SetupImage(&screen, eng_skeleton + i, atlas,
                            ATLAS_ENG_PIECES_BEGIN + i));
  }
}

static void
GameSetup(const char *title, int win_w, int win_h) {
  ErrLt0(SDL_Init(SDL_INIT_VIDEO));
  ErrIf0((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == IMG_INIT_PNG);

  win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, win_w, win_h,
                         SDL_WINDOW_SHOWN);
  ErrIf0(win);

  rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |
                            SDL_RENDERER_PRESENTVSYNC);
  ErrIf0(rend);

  GRAL_SetupScreen(&screen, rend, win_w, win_h);

  atlas = IMG_Load("atlas.png");
  ErrIf0(atlas);

  EngSkeletonSetup();

  ErrLt0(ATLAS_SetupImage(&screen, &fire_particle, atlas, ATLAS_CIRCLE_GRAD));

  ENG_Setup(&player, (vec2f) {0.0f, 0.0f}, &eng_skeleton, &fire_particle,
            1.0f/50.0f);

  PARTS_Setup(&parts);
}

static void
UpdateAndRender(Uint32 ms_now, Uint32 dt_ms) {
  ENG_Update(&player, &parts, ms_now, dt_ms);
  ENG_Render(&player, &screen);
  PARTS_UpdateAndRender(&parts, &screen, ms_now);
}

static void
ConsumeEvent(SDL_Event *e, Uint32 ms_now) {
  switch (e->type) {
    case SDL_KEYDOWN:
      switch (e->key.keysym.sym) {
        case SDLK_a:
          ENG_WalkSidewayLeft(&player);
          break;
        case SDLK_s:
          ENG_WalkBackward(&player);
          break;
        case SDLK_d:
          ENG_WalkSidewayRight(&player);
          break;
        case SDLK_w:
          ENG_WalkForward(&player);
          break;
        case SDLK_f:
          if (!e->key.repeat) {
            ENG_StartFiring(&player, ms_now);
          }
          break;
      }
      break;
    case SDL_KEYUP:
      switch (e->key.keysym.sym) {
        case SDLK_a:
          ENG_StopSidewayLeft(&player);
          break;
        case SDLK_s:
          ENG_StopBackward(&player);
          break;
        case SDLK_d:
          ENG_StopSidewayRight(&player);
          break;
        case SDLK_w:
          ENG_StopForward(&player);
          break;
        case SDLK_f:
          ENG_StopFiring(&player);
          break;
      }
      break;
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      int x, y;
      SDL_GetMouseState(&x, &y);
      y = GRAL_ScreenHeight(&screen) - 1 - y;
      ENG_WeaponFace(&player, (vec2f) {x, y});
      break;
    }
  }
}

static int
Run() {
  ENG_SetSpeed(&player, 0.3f);
  Uint32 last_update = SDL_GetTicks();
  for (;;) {
    Uint32 now = SDL_GetTicks();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return 0;
      }
      ConsumeEvent(&event, now);
    }
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
    SDL_RenderClear(rend);
    UpdateAndRender(now, now-last_update);
    last_update = now;
    SDL_RenderPresent(rend);
  }
}

int
main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  GameSetup("Walking Character", 800, 600);
  Run();
  Cleanup();
  return 0;
}
