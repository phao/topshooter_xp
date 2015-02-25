#include <assert.h>
#include <math.h>

#include "xMath.h"
#include "Graphical.h"
#include "Engineer.h"
#include "Particles.h"

/**
 * Automatically generated code. Don't change this. Check char_coords.py to
 * see how these numbers were generated.
 *
 * These values have to match the order of ENG_* pieces in ATLAS.
 */
static const vec2f
skeleton[ENG_NUM_BODY_PIECES] = {
  //head
  {0.0f, 0.0f},
  //shoulder left
  {-41.0f, -18.0f},
  //shoulder right
  {44.0f, -17.0f},
  //torso
  {1.0f, -25.0f},
  //arm left
  {-58.0f, -10.0f},
  //weapon
  {33.0f, 47.0f},
  //arm right
  {40.0f, 32.0f}
};

void
ENG_Setup(ENG_Character *eng,
          vec2f position,
          GRAL_Image (*images)[ENG_NUM_BODY_PIECES],
          GRAL_Image *fire_particle,
          float firing_freq_ms)
{
  eng->facing_unit = (vec2f) {1.0f, 0.0f};
  eng->facing_angle = 0.0f;
  eng->position = position;
  eng->speed = 0.0f;
  eng->forward = 0.0f;
  eng->right = 0.0f;
  eng->images = images;
  eng->anim_bouncing_ms = 0;
  eng->firing_since_ms = UINT32_MAX;
  eng->firing_freq_ms = firing_freq_ms;
  eng->fire_particle = fire_particle;
}

void
ENG_WeaponFace(ENG_Character *eng, vec2f facing_point) {
  if (eq_2f(facing_point, eng->position)) {
    // I just want to avoid division by 0 here. I don't need any
    // fancy floating point comparison technique.
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
   * To do the -90 degrees correction, we rotate the facing_unit
   * by -90 degrees.
   */
  vec2f *eng_fu = &eng->facing_unit;
  vec2f adjust = rot_2f(*eng_fu, (vec2f) {0.0f, -1.0f});
  vec2f weapon_pos = add_2f(eng->position,
                            rot_2f(skeleton[ENG_WEAPON], adjust));
  *eng_fu = normalize_2f(sub_2f(facing_point, weapon_pos));
  eng->facing_angle = atan2f(eng_fu->y, eng_fu->x);
}

static inline vec2f
WeaponTop(ENG_Character *eng) {
  vec2f adjust = rot_2f(eng->facing_unit, (vec2f) {0.0f, -1.0f});
  float weapon_height = GRAL_ImageHeight((*eng->images) + ENG_WEAPON);
  vec2f up_diff = add_2f(skeleton[ENG_WEAPON],
                         (vec2f) {0.0f, weapon_height*0.5f});
  return add_2f(eng->position, rot_2f(up_diff, adjust));
}


static void
Fire(ENG_Character *eng, PARTS *parts, Uint32 ms_now) {
  struct PARTS_BatchSetup batch_setup = {
    .start_position = WeaponTop(eng),
    .center_out_angle = eng->facing_angle,
    .spread_angle = XM_PI_f*0.01f,
    .ms_min_vel = 0.05f,
    .ms_max_vel = 0.3f,
    .color = (SDL_Color) {255, 85, 24, 255},
    .ms_start = ms_now,
    .ms_duration = 1000,
    .img = eng->fire_particle
  };
  PARTS_AddBatch(parts, &batch_setup);
}

void
ENG_Render(ENG_Character *eng, GRAL_Screen *screen) {
  float DEG_TO_RAD = XM_PI_f/180.0f;
  float bouncing_angle = (eng->anim_bouncing_ms/2 % 360) * DEG_TO_RAD;
  float sin = sinf(bouncing_angle);
  float cos = cosf(bouncing_angle);

  vec2f aux_skeleton[ENG_NUM_BODY_PIECES];
  memcpy(&aux_skeleton, &skeleton, sizeof skeleton);
  aux_skeleton[ENG_SHOULDER_LEFT].y += 0.5f*sin;
  aux_skeleton[ENG_SHOULDER_RIGHT].y += 0.5f*cos;
  aux_skeleton[ENG_ARM_LEFT].y += 4*cos;
  aux_skeleton[ENG_ARM_RIGHT].y += 4*sin;
  aux_skeleton[ENG_WEAPON].y += 4*sin;

  for (int i = ENG_NUM_BODY_PIECES-1; i >= 0; i--) {
    GRAL_DrawImage270RotRel(screen,
                            (*eng->images)+i,
                            add_2f(eng->position, aux_skeleton[i]),
                            eng->facing_angle,
                            eng->position);
  }
}

void
ENG_Update(ENG_Character *eng,
           PARTS *parts,
           Uint32 ms_now,
           Uint32 dt_ms)
{
  /*
   * Only update bouncing ms if moving. Comparison to floating points with
   * == and != is fine because of how they're put in there. Check the other
   * member functions (stop_forward for example).
   */
  if (eng->forward != 0.0f || eng->right != 0.0f) {
    eng->anim_bouncing_ms += dt_ms;
  }

  const float inv_sqrt2 = 1.0f/sqrtf(2.0f);

  // This is to prevent faster diagonal movement.
  float component_speed = (eng->forward != 0.0f && eng->right != 0.0f)
                          ? inv_sqrt2
                          : 1.0f;

  if (eng->forward != 0.0f) {
    assert(eng->forward == 1.0f || eng->forward == -1.0f);
    float direction_mod = eng->forward == -1.0f ? XM_HPI_f : XM_3HPI_f;
    float angle = eng->facing_angle + direction_mod;
    vec2f delta_pos = {0, component_speed*eng->speed*dt_ms};
    vec2f rot_angle = {cosf(angle), sinf(angle)};
    addto_2f(&eng->position, rot_2f(delta_pos, rot_angle));
  }

  if (eng->right != 0.0f) {
    assert(eng->right == 1.0f || eng->right == -1.0f);
    float angle = eng->facing_angle - eng->right*XM_PI_f*0.5f;
    vec2f delta_pos = {component_speed*eng->speed*dt_ms, 0};
    vec2f rot_angle = {cosf(angle), sinf(angle)};
    addto_2f(&eng->position, rot_2f(delta_pos, rot_angle));
  }

  if (eng->firing_since_ms != UINT32_MAX) {
    uint32_t inv_firing_freq_ms = 1.0f/eng->firing_freq_ms;
    int shots = (ms_now - eng->firing_since_ms)*eng->firing_freq_ms;
    if (shots >= 1) {
      do {
        Fire(eng, parts, ms_now);
        eng->firing_since_ms += inv_firing_freq_ms;
        --shots;
      } while (shots >= 1);
    }
  }
}
