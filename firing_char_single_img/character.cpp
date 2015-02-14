#include <cmath>

#include "xmath.hpp"
#include "graphical.hpp"
#include "character.hpp"

using xMATH::Float2;
using xMATH::PI;

namespace GAME {

Character::
Character(const Float2 position,
          GRAL::Image (* const images)[NUM_BODY_PIECES],
          GRAL::Image *fire_particle,
          float firing_freq_ms) noexcept
  : facing_unit_direction {1.0f, 0.0f},
    facing_angle {0.0f},
    position {position},
    speed {0.0f},
    forward {0.0f},
    right {0.0f},
    images {images},
    anim_bouncing_ms {0},
    firing_since_ms {UINT32_MAX},
    firing_freq_ms {firing_freq_ms},
    fire_particle {fire_particle}
{}

void
Character::
weapon_face(const Float2 facing_point) noexcept {
  if (facing_point == position) {
    // I just want to avoid division by 0 here. I don't need any
    // fancy floating point comparison-ish thing.
    return;
  }
  /*
   * To find the weapon's position, we need to take the facing angle,
   * and use the skeleton's data to rotate the weapon's position difference
   * (i.e. vector) to the facing angle minus 90 degrees.
   *
   * The reason why the -90 degrees correction is needed is because the
   * weapon's position difference is a vector telling how much to add to the
   * head's position to get to the weapon's position for when the character
   * is facing up (vector (0,1)). And the facing direction/angle is
   * based on the character facing right (vector (1,0)).
   *
   * Given this rotated vector, we add it to the position (that is of the head)
   * and we'll then have the weapon's position.
   *
   * To do the -90 degrees correction, we rotate the facing_unit_direction
   * by -90 degrees.
   */
  const Float2 adjust = rotate(facing_unit_direction, Float2{0.0f, -1.0f});
  const Float2 weapon_pos = position +
                            xMATH::rotate(skeleton[WEAPON], adjust);
  facing_unit_direction = normalize(facing_point - weapon_pos);
  facing_angle = std::atan2(facing_unit_direction.y(),
                            facing_unit_direction.x());
}

void
Character::
walk_sideway_right() noexcept {
  right = 1.0f;
}

void
Character::
stop_sideway_right() noexcept {
  // We're comparing floats for equality here, but that's okay because all
  // floats are set to fixed values. They're all either 0.0f, 1.0f or -1.0f.
  // There is no calculation, truncation, approximation, or anything else
  // going on.
  if (right == 1.0f) {
    right = 0.0f;
  }
}

void
Character::
walk_sideway_left() noexcept {
  right = -1.0f;
}

void
Character::
stop_sideway_left() noexcept {
  if (right == -1.0f) {
    right = 0.0f;
  }
}

void
Character::
walk_forward() noexcept {
  forward = 1.0f;
}

void
Character::
stop_forward() noexcept {
  if (forward == 1.0f) {
    forward = 0.0f;
  }
}

void
Character::
walk_backward() noexcept {
  forward = -1.0f;
}

void
Character::
stop_backward() noexcept {
  if (forward == -1.0f) {
    forward = 0.0f;
  }
}

void
Character::
set_speed(const float speed) noexcept {
  this->speed = speed;
}

void
Character::
start_firing(uint32_t ms_now) noexcept {
  firing_since_ms = ms_now;
}

void
Character::
stop_firing() noexcept {
  // UINT32_MAX signals "not firing";
  firing_since_ms = UINT32_MAX;
}

void
Character::
fire(ParticlesSystem *particles, uint32_t ms_now) noexcept {
  ParticlesBatchSetup batch_setup;
  batch_setup.start_position = weapon_top();
  batch_setup.center_out_angle = facing_angle;
  batch_setup.spread_angle = PI<float>()*0.01f;
  batch_setup.ms_min_vel = 0.05f;
  batch_setup.ms_max_vel = 0.3f;
  batch_setup.color = {255, 85, 24, 255};
  batch_setup.ms_start = ms_now;
  batch_setup.ms_duration = 1000;
  batch_setup.img = fire_particle;
  particles->add_batch(batch_setup);
}

void
Character::
render(GRAL::Screen *screen) noexcept {
  constexpr float DEG_TO_RAD {PI<float>()/180.0f};

  float bouncing_angle {(anim_bouncing_ms/2 % 360) * DEG_TO_RAD};
  float sin {std::sin(bouncing_angle)};
  float cos {std::cos(bouncing_angle)};

  // TODO? Make this const?
  Float2 aux_skeleton[NUM_BODY_PIECES];
  std::copy(skeleton, skeleton + NUM_BODY_PIECES, aux_skeleton);
  aux_skeleton[SHOULDER_LEFT].y() += 0.5f*sin;
  aux_skeleton[SHOULDER_RIGHT].y() += 0.5f*cos;
  aux_skeleton[ARM_LEFT].y() += 4*cos;
  aux_skeleton[ARM_RIGHT].y() += 4*sin;
  aux_skeleton[WEAPON].y() += 4*sin;

  for (int i = NUM_BODY_PIECES-1; i >= 0; i--) {
    screen->draw_image_270((*images)+i, position + aux_skeleton[i],
                           facing_angle, position);
  }
}

void
Character::
update(ParticlesSystem *particles,
       uint32_t ms_now,
       Uint32 dt_ms) noexcept
{
  // Only update bouncing ms if moving. Comparison to floating points with
  // == and != is fine because of how they're put in there. Check the other
  // member functions (stop_forward for example).
  if (forward != 0.0f || right != 0.0f) {
    anim_bouncing_ms += dt_ms;
  }

  // This is to prevent faster diagonal movement.
  const float component_speed([=]() {
    static const float inv_sqrt2 = 1.0f/std::sqrt(2.0f);
    return (forward != 0.0f && right != 0.0f) ? inv_sqrt2 : 1.0f;
  }());

  if (forward != 0.0f) {
    float angle {facing_angle + PI<float>()*(0.5f + (forward+1.0f)/2.0f)};
    Float2 delta_pos = Float2(0, component_speed*speed*dt_ms);
    Float2 rot_angle(std::cos(angle), std::sin(angle));
    position += rotate(delta_pos, rot_angle);
  }

  if (right != 0.0f) {
    float angle {facing_angle - right*PI<float>()*0.5f};
    Float2 delta_pos(component_speed*speed*dt_ms, 0);
    Float2 rot_angle(std::cos(angle), std::sin(angle));
    position += rotate(delta_pos, rot_angle);
  }

  if (firing_since_ms != UINT32_MAX) {
    uint32_t inv_firing_freq_ms = 1.0f/firing_freq_ms;
    int shots = (ms_now - firing_since_ms)*firing_freq_ms;
    if (shots >= 1) {
      do {
        fire(particles, ms_now);
        firing_since_ms += inv_firing_freq_ms;
        --shots;
      } while (shots >= 1);
    }
  }
}

xMATH::Float2
Character::
weapon_top() const noexcept {
  Float2 adjust = rotate(facing_unit_direction, Float2{0.0f, -1.0f});
  Float2 up_diff = skeleton[WEAPON] +
                   Float2{0.0f, (*images)[WEAPON].height()*0.5f};
  return position + xMATH::rotate(up_diff, adjust);
}

/**
 * Automatically generated code. Don't change this. Check char_coords.py to
 * see how these numbers were generated.
 */
const Float2
Character::
skeleton[NUM_BODY_PIECES] {
  //head
  Float2{0.0f, 0.0f},
  //shoulder left
  Float2{-41.0f, -18.0f},
  //shoulder right
  Float2{44.0f, -17.0f},
  //torso
  Float2{1.0f, -25.0f},
  //arm left
  Float2{-58.0f, -10.0f},
  //weapon
  Float2{33.0f, 47.0f},
  //arm right
  Float2{40.0f, 32.0f}
};

} // end of game
