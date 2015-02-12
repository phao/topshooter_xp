#ifndef PARTICLES_SYSTEM_HPP
#define PARTICLES_SYSTEM_HPP

#include <iostream>

#include <cstdint>
#include <cmath>
#include <cstdlib>

#include "xmath.hpp"
#include "xsdl.hpp"
#include "graphical.hpp"

inline float
RAND_01_f() {
  return float(rand())/float(RAND_MAX);
}

namespace game {

struct ParticlesBatchSetup {
  xmath::Float2 start_position;
  float center_out_angle;
  float spread_angle;
  float ms_min_vel;
  float ms_max_vel;
  xsdl::Color color;
  Uint32 ms_start;
  Uint32 ms_duration;
  gral::Image *img;
};

class ParticlesSystem {
public:
  ParticlesSystem()
    : batches_used {0}
  {}

  void
  add_batch(const ParticlesBatchSetup& setup) {
    if (batches_used == PARTICLE_BATCHES_MAX) {
      return;
    }

    auto& batch = batches[batches_used];
    batch.img = setup.img;
    batch.ms_duration = setup.ms_duration;
    batch.ms_start = setup.ms_start;
    batch.color = setup.color;
    batch.start_position = setup.start_position;

    const float base_angle = setup.center_out_angle - setup.spread_angle*0.5f;
    const float d_vel = setup.ms_max_vel - setup.ms_min_vel;

    for (auto& particle : batch.particles) {
      const float angle = base_angle + RAND_01_f()*setup.spread_angle;
      const float vel = setup.ms_min_vel + RAND_01_f()*d_vel;
      particle = {
        xmath::Float2 {std::cos(angle)*vel, std::sin(angle)*vel},
        float {RAND_01_f()*2.0f*xmath::PI<float>()}
      };
    }

    batches_used++;
  }

  void
  update_and_render(gral::Screen *screen, uint32_t ms_now) {
    int i = 0;

    while (i < batches_used) {
      ParticlesBatch &batch = batches[i];
      float dt = ms_now - batch.ms_start;

      if (dt > batch.ms_duration) {
        batch = batches[batches_used-1];
        batches_used--;
        continue;
      }

      float t = dt / batch.ms_duration;

      // -4t(t-1) goes from (0,0), (0.5, 1), (1, 0) in a quadratic fashion;
      // 0.5 being where it's at its max.
      float fact = -t*(t-1.0f)*4.0f;

      gral::AlphaModGuard alpha_mod_guard(batch.img, fact*255);
      gral::ColorModGuard color_mod_guard(batch.img, batch.color);
      gral::BlendModeGuard blend_mode_guard(batch.img, SDL_BLENDMODE_ADD);

      for (auto& particle : batch.particles) {
        xmath::Float2 d_pos = dt*particle.vel;
        xmath::Float2 pos = batch.start_position + d_pos;
        screen->draw_image(batch.img, pos, particle.angle);
      }

      i++;
    }
  }


private:
  enum {
    PARTICLE_BATCHES_MAX = 1 << 8,
    PARTICLES_PER_BATCH = 30,
  };

  struct ParticlesBatch {
    gral::Image *img;
    xmath::Float2 start_position;
    uint32_t ms_duration, ms_start;
    xsdl::Color color;
    struct {
      xmath::Float2 vel;
      float angle;
    } particles[PARTICLES_PER_BATCH];
  };

  ParticlesBatch batches[PARTICLE_BATCHES_MAX];
  int batches_used;
};

}
/*
static void
GenParticlesBatch(Uint32 ms_now) {
  float angle = RAND_01_f()*M_PI*2.0f;

  float x = WINDOW_WIDTH*RAND_01_f();
  float y = WINDOW_HEIGHT*RAND_01_f();

  float y = WINDOW_HEIGHT*RAND_01_f();

  float spread_angle = M_PI*0.8f;

  float min_ms_vel = 0.0f;
  float max_ms_vel = 0.03f;

  SDL_Color color = {255*RAND_01_f(),
                     255*RAND_01_f(),
                     255*RAND_01_f(),
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

  SDL_Color color = {64 + 32*RAND_01_f(),
                     24 + 12*RAND_01_f(),
                     12,
                     255};

  Uint32 duration = 1200;

  AddParticlesBatch(pos, angle, spread_angle, min_ms_vel, max_ms_vel,
                    color, ms_now, duration);
}
*/

#endif
