#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "xsdl.hpp"
#include "xsdl_image.hpp"
#include "graphical.hpp"
#include "character.hpp"
#include "particles_system.hpp"

#include "DBG.hpp"

using xmath::Float2;

namespace game {

class main {
public:
  main(const char * const title, const int width, const int height)
    : sdl {SDL_INIT_VIDEO},
      win {title, width, height},
      rend {&win, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC},
      screen {&rend, width, height},
      images {
        gral::Image {&screen, ximg::load("head.png")},
        gral::Image {&screen, ximg::load("shoulder_left.png")},
        gral::Image {&screen, ximg::load("shoulder_right.png")},
        gral::Image {&screen, ximg::load("torso.png")},
        gral::Image {&screen, ximg::load("arm_left.png")},
        gral::Image {&screen, ximg::load("weapon.png")},
        gral::Image {&screen, ximg::load("arm_right.png")}
      },
      fire_particle {&screen, ximg::load("fire_particle.png")},
      player {Float2(0, 0), &images, &fire_particle}
  {
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
      rend.set_draw_color(xsdl::BLACK);
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
            player.start_firing(ms_now);
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
  xsdl::SDL sdl;
  xsdl::Window win;
  xsdl::Renderer rend;
  gral::Screen screen;
  gral::Image images[Character::NUM_BODY_PIECES];
  ParticlesSystem particles;
  gral::Image fire_particle;
  Character player;
};

}

int
main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  try {
    return game::main("Walking Character", 800, 600).run();
  }
  catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << ".\n";
    return 1;
  }
}
