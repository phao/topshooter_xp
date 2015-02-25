#ifndef ENGINEER_H
#define ENGINEER_H

#include "Atlas.h"
#include "xMath.h"
#include "Graphical.h"
#include "Particles.h"

// These values have to match the order of ATLAS_ENG_* pieces.
enum {
  ENG_HEAD,
  ENG_SHOULDER_LEFT,
  ENG_SHOULDER_RIGHT,
  ENG_TORSO,
  ENG_ARM_LEFT,
  ENG_WEAPON,
  ENG_ARM_RIGHT,
  ENG_NUM_BODY_PIECES = ATLAS_ENG_NUM_PIECES
};

struct ENG_Character {
  vec2f facing_unit;
  float facing_angle;
  vec2f position;

  float speed;
  float forward;
  float right;

  GRAL_Image (*images)[ENG_NUM_BODY_PIECES];

  uint32_t anim_bouncing_ms;

  uint32_t firing_since_ms;
  float firing_freq_ms;
  GRAL_Image *fire_particle;
};

typedef struct ENG_Character ENG_Character;

void
ENG_Setup(ENG_Character *eng,
          vec2f position,
          GRAL_Image (*images)[ENG_NUM_BODY_PIECES],
          GRAL_Image *fire_particle,
          float firing_freq_ms);

/**
 * Sets the character facing so the weapon faces the given point.
 */
void
ENG_WeaponFace(ENG_Character *eng, vec2f facing_point);

static inline void
ENG_WalkSidewayRight(ENG_Character *eng) {
  eng->right = 1.0f;
}

static inline void
ENG_StopSidewayRight(ENG_Character *eng) {
  /*
   * We're comparing floats for equality here, but that's okay because all
   * floats are set to fixed values. They're all either 0.0f, 1.0f or -1.0f.
   * There is no calculation, truncation, approximation, or anything else
   * going on.
   */
  if (eng->right == 1.0f) {
    eng->right = 0.0f;
  }
}

static inline void
ENG_WalkSidewayLeft(ENG_Character *eng) {
  eng->right = -1.0f;
}

static inline void
ENG_StopSidewayLeft(ENG_Character *eng) {
  if (eng->right == -1.0f) {
    eng->right = 0.0f;
  }
}

static inline void
ENG_WalkForward(ENG_Character *eng) {
  eng->forward = 1.0f;
}

static inline void
ENG_StopForward(ENG_Character *eng) {
  if (eng->forward == 1.0f) {
    eng->forward = 0.0f;
  }
}

static inline void
ENG_WalkBackward(ENG_Character *eng) {
  eng->forward = -1.0f;
}

static inline void
ENG_StopBackward(ENG_Character *eng) {
  if (eng->forward == -1.0f) {
    eng->forward = 0.0f;
  }
}

static inline void
ENG_SetSpeed(ENG_Character *eng, float speed) {
  eng->speed = speed;
}

static inline void
ENG_StartFiring(ENG_Character *eng, Uint32 ms_now) {
  eng->firing_since_ms = ms_now;
}

static inline void
ENG_StopFiring(ENG_Character *eng) {
  // UINT32_MAX signals "not firing";
  eng->firing_since_ms = UINT32_MAX;
}

void
ENG_Render(ENG_Character *eng,
           GRAL_Screen *screen);

void
ENG_Update(ENG_Character *eng,
           PARTS *parts,
           Uint32 ms_now,
           Uint32 dt_ms);

#endif
