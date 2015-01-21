#ifndef PLAZMA_ARROW_SHOW_H
#define PLAZMA_ARROW_SHOW_H

#include <limits.h>

#include <SDL.h>

#include "Shared.h"

enum {
  PAS_DEFAULT_MS_TRAVEL_DURATION = 300,
  PAS_DEFAULT_MS_FIRE_DURATION = 50,
  PAS_DEFAULT_MS_SPLASH_DURATION = 100,
  PAS_DEFAULT_MS_FADE_DURATION = 100,
};

struct PAS_Anim {
  const struct SHR_Image *halo_img, *fire_img, *projectile_img, *splash_img;

  struct SHR_Float2 start_position;
  struct SHR_Float2 vel_ms;
  float angle;

  Uint32 ms_travel_duration;
  Uint32 ms_fire_duration;
  Uint32 ms_splash_duration;
  Uint32 ms_fade_duration;

  Uint32 ms_start;
  Uint32 ms_hit;
};

/**
 * Initializes the ms_* fields of a PAS_Anim struct to the default values
 * specified in the PlazmaArrowShow.h header file.
 */
inline void
PAS_AnimPutDefaultDurations(struct PAS_Anim *anim) {
  anim->ms_travel_duration = PAS_DEFAULT_MS_TRAVEL_DURATION;
  anim->ms_fire_duration = PAS_DEFAULT_MS_FIRE_DURATION;
  anim->ms_splash_duration = PAS_DEFAULT_MS_SPLASH_DURATION;
  anim->ms_fade_duration = PAS_DEFAULT_MS_FADE_DURATION;
}

inline void
PAS_AnimPutImages(struct PAS_Anim *anim,
                  struct SHR_Image *halo_img,
                  struct SHR_Image *fire_img,
                  struct SHR_Image *projectile_img,
                  struct SHR_Image *splash_img)
{
  anim->halo_img = halo_img;
  anim->fire_img = fire_img;
  anim->projectile_img = projectile_img;
  anim->splash_img = splash_img;
}

inline void
PAS_AnimStart(struct PAS_Anim *anim,
              struct SHR_Float2 start_position,
              struct SHR_Float2 vel_ms,
              float angle,
              Uint32 ms_start_when)
{
  anim->start_position = start_position;
  anim->vel_ms = vel_ms;
  anim->angle = angle;
  anim->ms_start = ms_start_when;
  anim->ms_hit = UINT32_MAX;
}

inline int
PAS_AnimIsSet(struct PAS_Anim *anim) {
  return anim->ms_start != UINT32_MAX;
}

inline void
PAS_AnimStop(struct PAS_Anim *anim) {
  anim->ms_hit = UINT32_MAX;
  anim->ms_start = UINT32_MAX;
}

inline void
PAS_AnimHit(struct PAS_Anim *anim, Uint32 ms_hit_when) {
  anim->ms_hit = ms_hit_when;
}

inline int
PAS_AnimIsHit(struct PAS_Anim *anim) {
  return anim->ms_hit < UINT32_MAX;
}

inline int
PAS_AnimIsFading(struct PAS_Anim *anim, Uint32 ms_now) {
  Uint32 dt = ms_now - anim->ms_start;
  return dt > anim->ms_travel_duration;
}

/**
 * It can be hit only if it wasn't already hit and if it's not fading.
 */
inline int
PAS_AnimCanBeHit(struct PAS_Anim *anim, Uint32 ms_now) {
  return !PAS_AnimIsHit(anim) && !PAS_AnimIsFading(anim, ms_now);
}

/**
 * @note Returns true even though it has already finished!
 */
inline int
PAS_AnimHasStarted(struct PAS_Anim *anim, Uint32 ms_now) {
  return ms_now >= anim->ms_start;
}

inline int
PAS_AnimStage(struct PAS_Anim *anim, Uint32 ms_now) {
  if (ms_now < anim->ms_start) {
    return -1;
  }

  Uint32 dt, total_duration;
  int is_hit;

  dt = ms_now - anim->ms_start;
  is_hit = ms_now > anim->ms_hit;

  if (is_hit) {
    total_duration = anim->ms_hit - anim->ms_start + anim->ms_splash_duration;
  }
  else {
    total_duration = anim->ms_travel_duration + anim->ms_fade_duration;
  }

  if (dt > total_duration) {
    return 1;
  }

  return 0;
}

void
PAS_AnimDraw(struct SHR_Screen *screen,
             struct PAS_Anim *anim,
             Uint32 ms_now);

#endif
