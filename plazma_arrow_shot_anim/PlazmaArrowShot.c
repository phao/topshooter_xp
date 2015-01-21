#include <math.h>
#include <assert.h>

#include "PlazmaArrowShot.h"
#include "DBG.h"

void
PAS_AnimDraw(struct SHR_Screen *screen,
             struct PAS_Anim *anim,
             Uint32 ms_now)
{
  if (ms_now < anim->ms_start) {
    return;
  }

  Uint32 dt, total_duration;
  int is_fading, is_hit;

  dt = ms_now - anim->ms_start;
  is_fading = dt > anim->ms_travel_duration;
  is_hit = ms_now > anim->ms_hit;

  if (is_hit) {
    total_duration = anim->ms_hit - anim->ms_start + anim->ms_splash_duration;
  }
  else {
    total_duration = anim->ms_travel_duration + anim->ms_fade_duration;
  }

  if (dt > total_duration) {
    dt = total_duration;
  }

  // Compute the sine and the cosine of the angle.
  // Cosine is in unit_vel.x, sine is in unit_vel.y.
  struct SHR_Float2 unit_vel;

  unit_vel = SHR_Normalize_f2(anim->vel_ms);

  assert(fabs(unit_vel.x*unit_vel.x + unit_vel.y*unit_vel.y - 1) < 0.01f);

  if (dt < anim->ms_fire_duration) {
    // The fire effect is still going on. We need to put in the fire image
    // and the halo.

    struct SHR_Float2 start_center;

    start_center = SHR_CenterForBaseAt(anim->fire_img,
                                       anim->start_position,
                                       unit_vel);

    SHR_DrawDownImage(screen, anim->fire_img,
                      start_center, anim->angle, 0);

    SHR_DrawDownImage(screen, anim->halo_img,
                      start_center, anim->angle, 0);
  }

  float speed = SHR_Norm_f2(anim->vel_ms);
  float height_dt_mod = anim->projectile_img->h/speed;

  if (is_hit) {
    // The arrow hit something somewhere in the past, and now we need to
    // put in the splash.

    Uint32 hit_dt;
    struct SHR_Float2 pos_diff, splash_top, splash_center;

    hit_dt = anim->ms_hit - anim->ms_start;
    pos_diff = SHR_Scale_f2(height_dt_mod + hit_dt, anim->vel_ms);
    splash_top = SHR_Add_f2(anim->start_position, pos_diff);
    splash_center = SHR_CenterForTopAt(anim->splash_img, splash_top, unit_vel);
    SHR_DrawUpImage(screen, anim->splash_img, splash_center, anim->angle, 0);
  }
  else {
    DBG_Decl(int, is_travelling);

    DBG_Eval(is_travelling = dt < anim->ms_travel_duration);
    SDL_assert(is_fading || is_travelling);

    // The arrow isn't fading, but is still travelling. We need to draw the
    // projectile at the right point.

    struct SHR_Float2 delta_pos, top_pos;

    delta_pos = SHR_Scale_f2(dt+height_dt_mod, anim->vel_ms);
    top_pos = SHR_Add_f2(anim->start_position, delta_pos);

    Uint8 alpha_mod;

    if (is_fading) {
      if (SDL_GetTextureAlphaMod(anim->projectile_img->tex, &alpha_mod) < 0) {
        // Ignore errors by just setting the alpha mod to maximum.
        alpha_mod = 255;
      }
      else {
        float fade_progress, t;

        fade_progress = dt - anim->ms_travel_duration;
        t = fade_progress/anim->ms_fade_duration;
        SDL_SetTextureAlphaMod(anim->projectile_img->tex, (1-t)*alpha_mod);
      }
    }

    struct SHR_Float2 proj_center;

    proj_center = SHR_CenterForTopAt(anim->projectile_img,
                                     top_pos, unit_vel);
    SHR_DrawUpImage(screen, anim->projectile_img,
                    proj_center, anim->angle, 0);

    if (is_fading) {
      SDL_SetTextureAlphaMod(anim->projectile_img->tex, alpha_mod);
    }
  }

  if (dt == total_duration) {
  }
}
