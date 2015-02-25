#ifndef PARTICLES_SYSTEM_H
#define PARTICLES_SYSTEM_H

#include <stdint.h>

#include "xMath.h"
#include "Graphical.h"

struct PARTS_BatchSetup {
  struct XM_Float2 start_position;
  float center_out_angle;
  float spread_angle;
  float ms_min_vel;
  float ms_max_vel;
  SDL_Color color;
  uint32_t ms_start;
  uint32_t ms_duration;
  GRAL_Image *img;
};

enum {
  PARTICLE_BATCHES_MAX = 1 << 8,
  PARTICLES_PER_BATCH = 30,
};

struct PARTS_Batch {
  GRAL_Image *img;
  struct XM_Float2 start_position;
  uint32_t ms_duration, ms_start;
  SDL_Color color;
  struct {
    struct XM_Float2 vel;
    float angle;
  } particles[PARTICLES_PER_BATCH];
};

struct PARTS {
  struct ParticlesBatch batches[PARTICLE_BATCHES_MAX];
  int batches_used;
};

typedef struct PARTS PARTS;

PARTS_Setup(PARTS *parts);

PARTS_AddBatch(PARTS *parts,
               const struct PARTS_BatchSetup *setup);

PARTS_UpdateAndRender(PARTS *parts,
                      GRAL_Screen *screen,
                      uint32_t ms_now);

#endif
