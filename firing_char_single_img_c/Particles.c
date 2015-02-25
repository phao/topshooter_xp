#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "xMath.h"
#include "Graphical.h"
#include "Particles.h"

static inline float
RAND_01_f() {
  return rand()/(float)RAND_MAX;
}

void
PARTS_AddBatch(PARTS *parts,
               const struct PARTS_BatchSetup* setup)
{
  if (parts->batches_used == PARTS_MAX_BATCHES) {
    return;
  }

  PARTS_Batch *batch = parts->batches + parts->batches_used;
  batch->img = setup->img;
  batch->ms_duration = setup->ms_duration;
  batch->ms_start = setup->ms_start;
  batch->color = setup->color;
  batch->start_position = setup->start_position;

  float base_angle = setup->center_out_angle - setup->spread_angle*0.5f;
  float d_vel = setup->ms_max_vel - setup->ms_min_vel;

  for (int i = 0; i < PARTS_PER_BATCH; i++) {
    float angle = base_angle + RAND_01_f()*setup->spread_angle;
    float vel = setup->ms_min_vel + RAND_01_f()*d_vel;
    batch->particles[i] = (PARTS_Each) {
      .vel = {cosf(angle)*vel, sinf(angle)*vel},
      .angle = RAND_01_f()*2.0f*XM_PI_f
    };
  }

  parts->batches_used++;
}

void
PARTS_UpdateAndRender(PARTS *parts,
                      GRAL_Screen *screen,
                      Uint32 ms_now)
{
  int i = 0;

  while (i < parts->batches_used) {
    PARTS_Batch *batch = parts->batches + i;
    float dt = ms_now - batch->ms_start;

    if (dt > batch->ms_duration) {
      *batch = parts->batches[parts->batches_used - 1];
      parts->batches_used--;
      continue;
    }

    float t = dt / batch->ms_duration;

    // -4t(t-1) goes from (0,0), (0.5, 1), (1, 0) in a quadratic fashion;
    // 0.5 being where it's at its max.
    float fact = -t*(t-1.0f)*4.0f;

    Uint8 old_alpha;
    SDL_Color old_color;
    SDL_BlendMode old_blend;

    GRAL_ImageAlphaMod(batch->img, &old_alpha);
    GRAL_ImageColorMod(batch->img, &old_color);
    GRAL_ImageBlendMode(batch->img, &old_blend);
    GRAL_SetImageAlphaMod(batch->img, fact*255);
    GRAL_SetImageColorMod(batch->img, batch->color);
    GRAL_SetImageBlendMode(batch->img, SDL_BLENDMODE_ADD);

    for (int i = 0; i < PARTS_PER_BATCH; i++) {
      PARTS_Each *p = batch->particles+i;
      vec2f pos = add_2f(batch->start_position, mul_2f(p->vel, dt));
      GRAL_DrawImage(screen, batch->img, pos, p->angle);
    }

    GRAL_SetImageAlphaMod(batch->img, old_alpha);
    GRAL_SetImageColorMod(batch->img, old_color);
    GRAL_SetImageBlendMode(batch->img, old_blend);

    i++;
  }
}
