#ifndef PARTICLES_H
#define PARTICLES_H

/**
 * Supposed use:
 * - Declare a PARTS variable.
 * - Call PARTS_Setup to setup the particles variable.
 * - You can then use PARTS_AddBatch and PARTS_UpdateAndRender.
 * - To call PARTS_AddBatch, you'll need to declare a variable of
 * struct PARTS_BatchSetup (not an "opaque-ish" type), and fill it in with
 * intended values.
 */

#include <stdint.h>

#include <SDL2/SDL.h>

#include "xMath.h"
#include "Graphical.h"

struct PARTS_BatchSetup {
  vec2f start_position;
  float center_out_angle;
  float spread_angle;
  float ms_min_vel;
  float ms_max_vel;
  SDL_Color color;
  Uint32 ms_start;
  Uint32 ms_duration;
  GRAL_Image *img;
};

enum {
  PARTS_MAX_BATCHES = 1 << 8,
  PARTS_PER_BATCH = 30,
};

struct PARTS_Batch {
  GRAL_Image *img;
  vec2f start_position;
  Uint32 ms_duration, ms_start;
  SDL_Color color;
  struct PARTS_Each {
    vec2f vel;
    float angle;
  } particles[PARTS_PER_BATCH];
};

typedef struct PARTS_Batch PARTS_Batch;

typedef struct PARTS_Each PARTS_Each;

struct PARTS {
  PARTS_Batch batches[PARTS_MAX_BATCHES];
  int batches_used;
};

typedef struct PARTS PARTS;

static inline void
PARTS_Setup(PARTS *parts) {
  parts->batches_used = 0;
}

void
PARTS_AddBatch(PARTS *parts,
               const struct PARTS_BatchSetup *setup);

void
PARTS_UpdateAndRender(PARTS *parts,
                      GRAL_Screen *screen,
                      Uint32 ms_now);

#endif
