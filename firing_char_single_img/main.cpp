#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "xSDL.hpp"
#include "xSDL_image.hpp"
#include "Graphical.hpp"
#include "EngCharacter.hpp"
#include "ParticlesSystem.hpp"
#include "Atlas.hpp"
#include "EngSkeleton.hpp"
#include "xMath.hpp"

using xMATH::Float2;

namespace GAME {

class Main {
public:
  Main(const char * const title, const int width, const int height)
    : sdl {SDL_INIT_VIDEO},
      win {title, width, height},
      rend {&win, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC},
      screen {&rend, width, height},
      atlas {xIMG::load("atlas.png")},
      skeleton {&screen, &atlas},
      fire_particle {&screen, &atlas, ATLAS::piece_geom(ATLAS::CIRCLE_GRAD)},
      player {Float2(0, 0), skeleton.images(), &fire_particle}
  {
    // I should learn how to use C++11's features for random numbers. I've read
    // in too many places that C's rand/srand are terrible.
    srand(time(0));
  }

  int
  run() {
    player.set_speed(0.3f);
    auto last_update = SDL_GetTicks();
    for (;;) {
      auto now = SDL_GetTicks();
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          return 0;
        }
        consume_event(event, now);
      }
      rend.set_draw_color(xSDL::BLACK);
      rend.clear();
      update_and_render(now, now-last_update);
      last_update = now;
      rend.present();
    }
  }

private:
  void
  update_and_render(uint32_t ms_now, uint32_t dt_ms) {
    player.update(&particles, ms_now, dt_ms);
    player.render(&screen);
    particles.update_and_render(&screen, ms_now);
  }

  void
  consume_event(SDL_Event &e, uint32_t ms_now) {
    switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_a:
            player.walk_sideway_left();
            break;
          case SDLK_s:
            player.walk_backward();
            break;
          case SDLK_d:
            player.walk_sideway_right();
            break;
          case SDLK_w:
            player.walk_forward();
            break;
          case SDLK_f:
            if (!e.key.repeat) {
              player.start_firing(ms_now);
            }
            break;
        }
        break;
      case SDL_KEYUP:
        switch (e.key.keysym.sym) {
          case SDLK_a:
            player.stop_sideway_left();
            break;
          case SDLK_s:
            player.stop_backward();
            break;
          case SDLK_d:
            player.stop_sideway_right();
            break;
          case SDLK_w:
            player.stop_forward();
            break;
          case SDLK_f:
            player.stop_firing();
            break;
        }
        break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        int x, y;
        SDL_GetMouseState(&x, &y);
        y = screen.height() - 1 - y;
        player.weapon_face(Float2{float(x), float(y)});
        break;
      }
    }
  }

private:
  xSDL::SDL sdl;
  xSDL::Window win;
  xSDL::Renderer rend;
  GRAL::Screen screen;
  xSDL::Surface atlas;
  EngSkeleton skeleton;
  ParticlesSystem particles;
  GRAL::Image fire_particle;
  EngCharacter player;
};

}

int
main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  try {
    return GAME::Main("Walking Character", 800, 600).run();
  }
  catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << ".\n";
    return 1;
  }
}
