#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "xmath.hpp"
#include "graphical.hpp"
#include "particles_system.hpp"

namespace game {

class Character {
public:
  enum {
    HEAD,
    SHOULDER_LEFT,
    SHOULDER_RIGHT,
    TORSO,
    ARM_LEFT,
    WEAPON,
    ARM_RIGHT,
    NUM_BODY_PIECES
  };

  static const xmath::Float2
  skeleton[NUM_BODY_PIECES];

  Character(const xmath::Float2 position,
            gral::Image (* const images)[NUM_BODY_PIECES],
            gral::Image *fire_particle,
            float firing_freq_ms = 1.0f/50.0f) noexcept;

  /**
   * Sets the character facing so the weapon faces the given point.
   */
  void
  weapon_face(const xmath::Float2 facing_point) noexcept;

  void
  walk_sideway_right() noexcept;

  void
  stop_sideway_right() noexcept;

  void
  walk_sideway_left() noexcept;

  void
  stop_sideway_left() noexcept;

  void
  walk_forward() noexcept;

  void
  stop_forward() noexcept;

  void
  walk_backward() noexcept;

  void
  stop_backward() noexcept;

  void
  set_speed(const float speed) noexcept;

  void
  start_firing(uint32_t ms_now);

  void
  stop_firing();

  void
  render(gral::Screen *screen) noexcept;

  void
  fire(ParticlesSystem *particles, uint32_t ms_now) noexcept;

  void
  update(ParticlesSystem *particles,
         uint32_t ms_now,
         uint32_t dt_ms) noexcept;

  xmath::Float2
  weapon_top() const noexcept;

private:
  xmath::Float2 facing_unit_direction {1.0f, 0.0f};
  float facing_angle {0.0f};
  xmath::Float2 position;

  float speed;
  float forward;
  float right;

  gral::Image (* const images)[NUM_BODY_PIECES];

  uint32_t anim_bouncing_ms;

  uint32_t firing_since_ms;
  float firing_freq_ms;
  gral::Image *fire_particle;
};

} // end of game

#endif
