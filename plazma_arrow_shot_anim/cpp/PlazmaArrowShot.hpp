#ifndef PLAZMA_ARROW_SHOW_H
#define PLAZMA_ARROW_SHOW_H

#include <cfloat>
#include <cmath>

#include <SDL.h>

#include "Shared.hpp"

namespace PAS {

class Animation {
  using SHR::Float2;
  using SHR::Screen;
  using SHR::Image;
  using std::abs;

  public:
    enum {
      STAGE_NOT_STARTED = -1,
      STAGE_RUNNING = 0,
      STAGE_ENDED = 1
    };

    enum {
      DEFAULT_MS_TRAVEL_DURATION = 300,
      DEFAULT_MS_FIRE_DURATION = 100,
      DEFAULT_MS_SPLASH_DURATION = 100,
      DEFAULT_MS_FADE_DURATION = 100,
    };

    Animation(const Image &halo_img,
              const Image &fire_img,
              const Image &projectile_img,
              const Image &splash_img,
              Uint32 ms_fire_duration = DEFAULT_MS_FIRE_DURATION,
              Uint32 ms_travel_duration = DEFAULT_MS_TRAVEL_DURATION,
              Uint32 ms_fade_duration = DEFAULT_MS_FADE_DURATION,
              Uint32 ms_splash_duration = DEFAULT_MS_SPLASH_DURATION)
      : halo_img(halo_img),
        fire_img(fire_img),
        projectile_img(projectile_img),
        splash_img(splash_img),
        ms_fire_duration(ms_fire_duration),
        ms_travel_duration(ms_travel_duration),
        ms_fade_duration(ms_fade_duration),
        ms_splash_duration(ms_splash_duration)
    {}

    Animation(const Animation& from) = delete;
    Animation& operator = (const Animation& from) = delete;

    Animation(const Animation&& from) = default;
    Animation&& operator = (const Animation&& from) = default;

    void
    Start(Float2 start_position,
          Float2 vel_ms,
          float angle,
          Uint32 ms_start_when)
    {
      this->start_position = start_position;
      this->vel_ms = vel_ms;
      this->angle = angle;
      ms_start = ms_start_when;
      ms_hit = UINT32_MAX;
    }

    void
    Stop() {
      ms_start = UINT32_MAX;
      ms_hit = UINT32_MAX;
    }

    void
    Hit(Uint32 ms_hit_when) {
      ms_hit = ms_hit_when;
    }

    bool
    IsFading(Uint32 ms_now) const {
      Uint32 dt = ms_now - ms_start;
      return dt > ms_travel_duration;
    }

    int
    Stage(Uint32 ms_now) const {
      if (ms_now < ms_start) {
        return STAGE_NOT_STARTED;
      }

      Uint32 dt, total_duration;
      int is_hit;

      dt = ms_now - ms_start;
      is_hit = ms_now > ms_hit;

      if (is_hit) {
        total_duration = ms_hit - ms_start + ms_splash_duration;
      }
      else {
        total_duration = ms_travel_duration + ms_fade_duration;
      }

      if (dt > total_duration) {
        return STAGE_ENDED;
      }

      return STAGE_RUNNING;
    }

    void
    Draw(Screen &screen, Uint32 ms_now) {
      if (ms_now < ms_start) {
        return;
      }

      Uint32 dt = ms_now - ms_start;
      bool is_fading = dt > ms_travel_duration;
      bool is_hit = ms_now > ms_hit;

      Uint32 total_duration;

      if (is_hit) {
        total_duration = ms_hit - ms_start + ms_splash_duration;
      }
      else {
        total_duration = ms_travel_duration + ms_fade_duration;
      }

      if (dt > total_duration) {
        dt = total_duration;
      }

      // Compute the sine and the cosine of the angle.
      // Cosine is in unit_vel.x, sine is in unit_vel.y.
      Float2 unit_vel(Normalize(vel_ms));

      assert(abs(unit_vel.x*unit_vel.x + unit_vel.y*unit_vel.y - 1) < 0.01f);

      if (dt < ms_fire_duration) {
        // The fire effect is still going on. We need to put in the fire image
        // and the halo.

        Float2 start_center(fire_img.CenterForBaseAt(start_position,
                                                     unit_vel));
        screen.DrawDownImage(fire_img, start_center, angle, 0);
        screen.DrawDownImage(halo_img, start_center, angle, 0);
      }

      if (is_hit) {
        // The arrow hit something somewhere in the past, and now we need to
        // put in the splash.

        Uint32 hit_dt = ms_hit - ms_start;
        Float2 pos_diff(hit_dt*vel_ms);
        Float2 splash_top(start_position + pos_diff);
        Float2 splash_center(splash_img.CenterForBaseAt(splash_top,
                                                        unit_vel));
        screen.DrawDownImage(splash_img, splash_center, angle, 0);
      }
      else {
        DBG_Decl(int, is_travelling);

        DBG_Eval(is_travelling = dt < ms_travel_duration);
        SDL_assert(is_fading || is_travelling);

        // The arrow isn't fading, but is still travelling. We need to draw the
        // projectile at the right point.

        Float2 delta_pos(dt*vel_ms);
        Float2 base_pos(start_position + delta_pos);

        Uint8 alpha_mod;

        if (is_fading) {
          if (SDL_GetTextureAlphaMod(projectile_img->tex, &alpha_mod) < 0) {
            // Ignore errors by just setting the alpha mod to maximum.
            alpha_mod = 255;
          }
          else {
            float fade_progress = dt - ms_travel_duration;
            float t = fade_progress/ms_fade_duration;
            SDL_SetTextureAlphaMod(projectile_img->tex, (1-t)*alpha_mod);
          }
        }

        Float2 proj_center = projectile_img.CenterForBaseAt(base_pos,
          unit_vel);
        screen.DrawUpImage(projectile_img, proj_center, angle, 0);

        if (is_fading) {
          SDL_SetTextureAlphaMod(projectile_img->tex, alpha_mod);
        }
      }
    }

  private:
    const shr::Image &halo_img, &fire_img, &projectile_img, &splash_img;

    shr::Float2 start_position;
    shr::Float2 vel_ms;
    float angle;

    Uint32 ms_travel_duration;
    Uint32 ms_fire_duration;
    Uint32 ms_splash_duration;
    Uint32 ms_fade_duration;

    Uint32 ms_start;
    Uint32 ms_hit;
};

} // end of namespace pas
#endif
