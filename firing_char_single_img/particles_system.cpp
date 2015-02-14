#include <iostream>

#include <cstdint>
#include <cmath>
#include <cstdlib>

#include "xmath.hpp"
#include "xsdl.hpp"
#include "graphical.hpp"
#include "particles_system.hpp"

inline float
RAND_01_f() {
  return float(rand())/float(RAND_MAX);
}

namespace GAME {

ParticlesSystem::
ParticlesSystem() noexcept
  : batches_used {0}
{}

void
ParticlesSystem::
add_batch(const ParticlesBatchSetup& setup) noexcept {
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
      xMATH::Float2 {std::cos(angle)*vel, std::sin(angle)*vel},
      float {RAND_01_f()*2.0f*xMATH::PI<float>()}
    };
  }

  batches_used++;
}

void
ParticlesSystem::
update_and_render(GRAL::Screen *screen, uint32_t ms_now) {
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

    GRAL::AlphaModGuard alpha_mod_guard(batch.img, fact*255);
    GRAL::ColorModGuard color_mod_guard(batch.img, batch.color);
    GRAL::BlendModeGuard blend_mode_guard(batch.img, SDL_BLENDMODE_ADD);

    for (const auto& particle : batch.particles) {
      xMATH::Float2 d_pos = dt*particle.vel;
      xMATH::Float2 pos = batch.start_position + d_pos;
      screen->draw_image(batch.img, pos, particle.angle);
    }

    i++;
  }
}

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
