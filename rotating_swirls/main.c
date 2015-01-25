#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PI_f 3.141592f

enum {
  SCREEN_WIDTH = 800,
  SCREEN_HEIGHT = 600,
  SCREEN_CENTER_X = SCREEN_WIDTH/2,
  SCREEN_CENTER_Y = SCREEN_HEIGHT/2,
};

static SDL_Window *win;
static SDL_Renderer *rend;

static SDL_Texture *swirl_tex;
static SDL_Rect swirl_rect;

enum {
  SWIRL_MOVEMENT_RAD = 10 // pixels
};

static const float SWIRL_TEXTURE_ALPHA_MOD = 128;

static void
Cleanup(void) {
  if (swirl_tex) SDL_DestroyTexture(swirl_tex);
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

static void
Init(void) {
  ExitLt0(SDL_Init(SDL_INIT_VIDEO));

  win = SDL_CreateWindow("Rotating Swirls", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN);

  ExitIf0(win);

  int render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win, -1, render_flags);

  ExitIf0(rend);

  int img_load_flags = IMG_INIT_PNG;
  int mask_loaded = IMG_Init(img_load_flags) & img_load_flags;

  ExitIf0(mask_loaded);

  SDL_Surface *swirl_surface = IMG_Load("blue_swirl.png");

  ExitIf0(swirl_surface);

  swirl_rect.w = swirl_surface->w/4;
  swirl_rect.h = swirl_surface->h/4;

  swirl_tex = SDL_CreateTextureFromSurface(rend, swirl_surface);
  SDL_FreeSurface(swirl_surface);

  ExitIf0(swirl_tex);

  int status;

  status = SDL_SetTextureBlendMode(swirl_tex, SDL_BLENDMODE_ADD);
  ExitLt0(status);
  status = SDL_SetTextureAlphaMod(swirl_tex, SWIRL_TEXTURE_ALPHA_MOD);
  ExitLt0(status);
}

static void
RenderSwirl(float sin_angle, float cos_angle,
            float rad_factor, float angle)
{
  int status;

  float base_x = SCREEN_CENTER_X - swirl_rect.w/2;
  float rot_x = cos_angle*(SWIRL_MOVEMENT_RAD*rad_factor);
  swirl_rect.x =  base_x + rot_x;

  float base_y = SCREEN_CENTER_Y - swirl_rect.h/2;
  float rot_y = sin_angle*(SWIRL_MOVEMENT_RAD*rad_factor);
  swirl_rect.y = base_y + rot_y;

  status = SDL_RenderCopyEx(rend, swirl_tex, 0, &swirl_rect,
                            -angle, 0, SDL_FLIP_NONE);
  ExitLt0(status);
}

static inline float
DegreesToRadians(float degrees) {
  return degrees/180.0f * PI_f;
}

static void
Animate(Uint32 ms_since_start) {
  enum {
    NUM_SWIRLS = 64
  };
/*
  float time_degrees = ms_since_start/5 % 360;
  float alpha_mod01 = 0.5f + cosf(DegreesToRadians(time_degrees))/2.0f;
  int status = SDL_SetTextureAlphaMod(swirl_tex, 255*alpha_mod01);

  ExitLt0(status);
*/
  const float PI_F = 3.141592f;

  for (int i = 0; i < NUM_SWIRLS; i++) {
    float angle_degrees = i*ms_since_start/(4*NUM_SWIRLS) % 360;
    float rad_factor = 15.0f*i/NUM_SWIRLS;
    float angle_radians = DegreesToRadians(angle_degrees);

    RenderSwirl(sinf(angle_radians),
                cosf(-PI_F/2.0f + 2*angle_radians),
                rad_factor,
                angle_degrees);
  }
}

static void
AnimLoop(void) {
  SDL_Event e;

  for (;;) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }

    int status = SDL_SetRenderDrawColor(rend, 16, 16, 16, 16);
    ExitLt0(status);

    status = SDL_RenderClear(rend);
    ExitLt0(status);

    Animate(SDL_GetTicks());
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
