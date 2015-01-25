#include <stdio.h>

#include <math.h>
#include <assert.h>

#include <SDL.h>

#include "Shared.h"
#include "PlazmaArrowShot.h"
#include "DBG.h"

static Uint8
FadeIn(struct SHR_Image *img, Uint32 fade_progress, Uint32 fade_duration) {
  Uint8 alpha_mod;

  if (SDL_GetTextureAlphaMod(img->tex, &alpha_mod) < 0) {
    // Ignore errors by just setting the alpha mod to maximum.
    alpha_mod = 255;
  }
  else {
    const float t = fade_progress/(float)fade_duration;

    assert(t >= 0 && t <= 1);

    SDL_SetTextureAlphaMod(img->tex, (1-t)*alpha_mod);
  }

  return alpha_mod;
}

int
PAS_AnimDraw(struct SHR_Screen *screen,
             struct PAS_Anim *anim,
             Uint32 ms_now)
{
  if (ms_now < anim->ms_start) {
    const Uint32 total_time = anim->ms_travel_duration +
                              anim->ms_fade_duration;
    return anim->ms_start - ms_now + total_time;
  }

  const Uint32 ms_elapsed = ms_now - anim->ms_start;
  const int is_fading = ms_elapsed > anim->ms_travel_duration;
  const int is_hit = ms_now > anim->ms_hit;

  const Uint32 total_duration = is_hit
                                ? anim->ms_hit - anim->ms_start +
                                  anim->ms_splash_duration
                                : anim->ms_travel_duration +
                                  anim->ms_fade_duration;

  const Uint32 dt = ms_elapsed > total_duration
                    ? total_duration
                    : ms_elapsed;

  // Compute the sine and the cosine of the angle.
  // Cosine is in unit_vel.x, sine is in unit_vel.y.
  const struct SHR_Float2 unit_vel = SHR_Normalize_f2(anim->vel_ms);

  assert(fabs(unit_vel.x*unit_vel.x + unit_vel.y*unit_vel.y - 1) < 0.01f);

  if (dt < anim->ms_fire_duration) {
    // The fire effect is still going on. We need to put in the fire image
    // and the halo.

    const struct SHR_Float2
    fire_center = SHR_CenterForBaseAt(anim->fire_img,
                                      anim->start_position,
                                      unit_vel);

    const struct SHR_Float2
    halo_center = SHR_CenterForBaseAt(anim->halo_img,
                                      anim->start_position,
                                      unit_vel);

    // Should we make an effort to reduce this into a single fade-in call?
    // Does it matter?

    const Uint8 old_fire_alpha_mod = FadeIn(anim->fire_img, dt,
                                            anim->ms_fire_duration);

    const Uint8 old_halo_alpha_mod = FadeIn(anim->halo_img, dt,
                                            anim->ms_fire_duration);

    SHR_DrawUpImage(screen, anim->fire_img,
                    fire_center, anim->angle, 0);

    SHR_DrawUpImage(screen, anim->halo_img,
                    halo_center, anim->angle, 0);

    SDL_SetTextureAlphaMod(anim->fire_img->tex, old_fire_alpha_mod);
    SDL_SetTextureAlphaMod(anim->halo_img->tex, old_halo_alpha_mod);
  }

  const float speed = SHR_Norm_f2(anim->vel_ms);
  const float height_dt_mod = anim->projectile_img->h/speed;

  if (is_hit) {
    // The arrow hit something somewhere in the past, and now we need to
    // put in the splash.

    const Uint32 non_hit_time = anim->ms_hit - anim->ms_start;
    const Uint32 hit_dt = dt - non_hit_time;

    const struct SHR_Float2
    pos_diff = SHR_Scale_f2(height_dt_mod + non_hit_time, anim->vel_ms);

    const struct SHR_Float2
    splash_top = SHR_Add_f2(anim->start_position, pos_diff);

    const struct SHR_Float2
    splash_center = SHR_CenterForTopAt(anim->splash_img, splash_top, unit_vel);

    const Uint8 old_alpha_mod = FadeIn(anim->splash_img, hit_dt,
                                       anim->ms_splash_duration);

    SHR_DrawUpImage(screen, anim->splash_img, splash_center, anim->angle, 0);

    SDL_SetTextureAlphaMod(anim->splash_img->tex, old_alpha_mod);
  }
  else {
    DBG_Decl(int, is_travelling);

    DBG_Eval(is_travelling = dt < anim->ms_travel_duration);
    SDL_assert(is_fading || is_travelling);

    // The arrow isn't fading, but is still travelling. We need to draw the
    // projectile at the right point.

    const struct SHR_Float2
    delta_pos = SHR_Scale_f2(dt+height_dt_mod, anim->vel_ms);

    const struct SHR_Float2
    top_pos = SHR_Add_f2(anim->start_position, delta_pos);

    const Uint8 old_alpha_mod = is_fading
                                ? FadeIn(anim->projectile_img,
                                         dt - anim->ms_travel_duration,
                                         anim->ms_fade_duration)
                                : 0;

    const struct SHR_Float2
    proj_center = SHR_CenterForTopAt(anim->projectile_img,
                                     top_pos, unit_vel);

    SHR_DrawUpImage(screen, anim->projectile_img,
                    proj_center, anim->angle, 0);

    if (is_fading) {
      SDL_SetTextureAlphaMod(anim->projectile_img->tex, old_alpha_mod);
    }
  }

  assert(dt <= total_duration);

  return total_duration - dt;
}
