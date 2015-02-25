#ifndef PARTICLES_SYSTEM_HPP
#define PARTICLES_SYSTEM_HPP

#include <cstdint>

#include "xMath.hpp"
#include "xSDL.hpp"
#include "Graphical.hpp"

namespace GAME {

struct ParticlesBatchSetup {
  xMATH::Float2 start_position;
  float center_out_angle;
  float spread_angle;
  float ms_min_vel;
  float ms_max_vel;
  xSDL::Color color;
  Uint32 ms_start;
  Uint32 ms_duration;
  GRAL::Image *img;
};

class ParticlesSystem {
public:
  ParticlesSystem() noexcept;

  void
  add_batch(const ParticlesBatchSetup& setup) noexcept;

  void
  update_and_render(GRAL::Screen *screen, uint32_t ms_now);

private:
  enum {
    PARTICLE_BATCHES_MAX = 1 << 8,
    PARTICLES_PER_BATCH = 30,
  };

  struct ParticlesBatch {
    GRAL::Image *img;
    xMATH::Float2 start_position;
    uint32_t ms_duration, ms_start;
    xSDL::Color color;
    struct {
      xMATH::Float2 vel;
      float angle;
    } particles[PARTICLES_PER_BATCH];
  };

  ParticlesBatch batches[PARTICLE_BATCHES_MAX];
  int batches_used;
};

}

#endif
