#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Shared.h"

enum {
  WINDOW_WIDTH = 800,
  WINDOW_HEIGHT = 600
};

static SDL_Window *win;
static struct SHR_Screen screen;
static struct SHR_Image ball_img;

enum {
  PARTICLE_BATCHES_MAX = 1 << 8,
  PARTICLES_PER_BATCH = 30,
};

struct ParticleBatch {
  Uint32 ms_start, ms_duration;
  struct SHR_Float2 start_position;
  SDL_Color color;
  struct {
    struct SHR_Float2 pos;
    float angle;
  } particles[PARTICLES_PER_BATCH];
};

static struct ParticleBatch particles_batches[PARTICLE_BATCHES_MAX];
static int particles_batches_used;

static void
Cleanup(void) {
  SHR_DestroyImage(&ball_img);
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

#define ErrIf0(v) if ((v) == 0) ErrorExit(); else (void) 0
#define ErrLt0(v) if ((v) < 0) ErrorExit(); else (void) 0

void
Init(void) {
  ErrLt0(SDL_Init(SDL_INIT_VIDEO));

  win = SDL_CreateWindow("Sparts003", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                         SDL_WINDOW_SHOWN);

  ErrIf0(win);

  SDL_Renderer *rend;
  rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |
                            SDL_RENDERER_PRESENTVSYNC);

  ErrIf0(rend);
  ErrIf0(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG);
  ErrLt0(SHR_LoadImage(rend, "particle.png", &ball_img));

  SDL_SetTextureBlendMode(ball_img.tex, SDL_BLENDMODE_ADD);

  SHR_ScreenSetup(&screen, rend, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void
AddParticlesBatch(struct SHR_Float2 start_position,
                  float center_out_angle,
                  float spread_angle,
                  float ms_min_vel,
                  float ms_max_vel,
                  SDL_Color color,
                  Uint32 ms_start,
                  Uint32 ms_duration)
{
  if (particles_batches_used == PARTICLE_BATCHES_MAX) {
    return;
  }

  struct ParticleBatch *pb = particles_batches + particles_batches_used;

  pb->ms_duration = ms_duration;
  pb->ms_start = ms_start;
  pb->start_position = start_position;
  pb->color = color;

  float base_angle = center_out_angle - spread_angle/2.0f;
  float d_vel = ms_max_vel - ms_min_vel;
  for (int i = 0; i < PARTICLES_PER_BATCH; i++) {
    struct SHR_Float2 *p_ms_vel = &pb->particles[i].pos;
    float angle = base_angle + (rand()/(float)RAND_MAX)*spread_angle;
    float vel = ms_min_vel + (rand()/(float)RAND_MAX)*d_vel;
    p_ms_vel->x = cosf(angle)*vel;
    p_ms_vel->y = sinf(angle)*vel;

    pb->particles[i].angle = (rand()/(float)RAND_MAX)*2.0f*M_PI;
  }

  particles_batches_used++;
}

static void
GenParticlesBatch(Uint32 ms_now) {
  float angle = (rand()/(float)RAND_MAX)*M_PI*2.0f;

  float x = WINDOW_WIDTH*(rand()/(float)RAND_MAX);
  float y = WINDOW_HEIGHT*(rand()/(float)RAND_MAX);

  struct SHR_Float2 pos = {x, y};

  float spread_angle = M_PI*0.8f;

  float min_ms_vel = 0.0f;
  float max_ms_vel = 0.03f;

  SDL_Color color = {255*(rand()/(float)RAND_MAX),
                     255*(rand()/(float)RAND_MAX),
                     255*(rand()/(float)RAND_MAX),
                     255};

  Uint32 duration = 500;

  AddParticlesBatch(pos, angle, spread_angle, min_ms_vel, max_ms_vel,
                    color, ms_now, duration);
}

static void
GenFixedFire(Uint32 ms_now) {
  float angle = M_PI/4.0f;

  struct SHR_Float2 pos = {400, 300};

  float spread_angle = M_PI*0.1f;

  float min_ms_vel = 0.0f;
  float max_ms_vel = 0.2f;

  SDL_Color color = {64 + 32*(rand()/(float)RAND_MAX),
                     24 + 12*(rand()/(float)RAND_MAX),
                     12,
                     255};

  Uint32 duration = 1200;

  AddParticlesBatch(pos, angle, spread_angle, min_ms_vel, max_ms_vel,
                    color, ms_now, duration);
}

static void
UpdateAndRenderParticles(Uint32 ms_now) {
  int i = 0;

  while (i < particles_batches_used) {
    struct ParticleBatch *pb = particles_batches + i;
    float dt = ms_now - pb->ms_start;

    if (dt > pb->ms_duration) {
      *pb = particles_batches[particles_batches_used-1];
      particles_batches_used--;
      continue;
    }

    float t = dt / pb->ms_duration;

    // -4t(t-1) goes from (0,0), (0.5, 1), (1, 0) in a quadratic fashion;
    // 0.5 being where it's at its max.
    float fact = -t*(t-1)*4.0;

    SDL_SetTextureAlphaMod(ball_img.tex, fact*255);

    SDL_SetTextureColorMod(ball_img.tex, pb->color.r, pb->color.g,
                           pb->color.b);

    for (int j = 0; j < PARTICLES_PER_BATCH; j++) {
      struct SHR_Float2 d_pos = SHR_Scale_f2(dt, pb->particles[j].pos);
      struct SHR_Float2 pos = SHR_Add_f2(pb->start_position, d_pos);

      SHR_DrawRightImage(&screen, &ball_img, pos, pb->particles[j].angle, 0);
    }

    i++;
  }

  SDL_SetTextureAlphaMod(ball_img.tex, 255);
  SDL_SetTextureColorMod(ball_img.tex, 255, 255, 255);
}

static void
UpdateAndRender(Uint32 ms_now) {
  (void) ms_now;

  static double acc_time;
  static Uint64 n_measures;

  if (acc_time > 2) {
    printf("%f secs (avg)\n", acc_time/n_measures);
    acc_time = 0;
    n_measures = 0;
  }

  int n_particles = (rand()/(float)RAND_MAX)*PARTICLE_BATCHES_MAX*0.004f;

  for (int i = 0; i < n_particles; i++) {
    GenParticlesBatch(ms_now);
  }
  if (rand()%100 < 30) {
    GenFixedFire(ms_now);
  }

  Uint64 before = SDL_GetPerformanceCounter();
  UpdateAndRenderParticles(ms_now);
  Uint64 after = SDL_GetPerformanceCounter();
  n_measures++;
  acc_time += (after - before) / (double) SDL_GetPerformanceFrequency();
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
    ErrLt0(status);

    status = SDL_RenderClear(screen.rend);
    ErrLt0(status);

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
