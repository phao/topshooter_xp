#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "xSDL.hpp"
#include "xSDL_image.hpp"
#include "Shared.hpp"

#include "DBG.hpp"

using SHR::Float2;

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

  static const Float2
  skeleton[NUM_BODY_PIECES];

  Character(Float2 position, SHR::Image (*images)[NUM_BODY_PIECES])
    : position(position),
      images(images)
  {}

  void
  face(Float2 facing_point) {
    if (facing_point == position) {
      // I just want to avoid division by 0 here. I don't need any
      // fancy floating point comparison-ish thing.
      return;
    }
    facing_unit_direction = normalize(facing_point - position);
    facing_angle = std::atan2(facing_unit_direction.y(),
                              facing_unit_direction.x());
  }

  void
  walk_sideway_right() {
    right = 1.0f;
  }

  void
  stop_sideway_right() {
    // We're comparing floats for equality here, but that's okay because all
    // floats are set to fixed values. They're all either 0.0f, 1.0f or -1.0f.
    // There is no calculation, truncation, approximation, or anything else
    // going on.
    if (right == 1.0f) {
      right = 0.0f;
    }
  }

  void
  walk_sideway_left() {
    right = -1.0f;
  }

  void
  stop_sideway_left() {
    if (right == -1.0f) {
      right = 0.0f;
    }
  }

  void
  walk_forward() {
    forward = 1.0f;
  }

  void
  stop_forward() {
    if (forward == 1.0f) {
      forward = 0.0f;
    }
  }

  void
  walk_backward() {
    forward = -1.0f;
  }

  void
  stop_backward() {
    if (forward == -1.0f) {
      forward = 0.0f;
    }
  }

  void
  set_speed(float speed) {
    this->speed = speed;
  }

  void
  render(SHR::Screen *screen) {
    Float2 aux_skeleton[NUM_BODY_PIECES];
    std::copy(skeleton, skeleton + NUM_BODY_PIECES, aux_skeleton);

    float bouncing_angle = anim_bouncing_ms/2 % 360;
    bouncing_angle /= 180;
    bouncing_angle *= M_PI;

    float sin = std::sin(bouncing_angle);
    float cos = std::cos(bouncing_angle);
    aux_skeleton[SHOULDER_LEFT].y() += 2*sin;
    aux_skeleton[SHOULDER_RIGHT].y() += 2*sin;
    aux_skeleton[ARM_LEFT].y() += 3*cos;
    aux_skeleton[ARM_RIGHT].y() += 3*sin;
    aux_skeleton[WEAPON].y() += 3*sin;

    for (int i = NUM_BODY_PIECES-1; i >= 0; i--) {
      screen->draw_image_270((*images)+i, position + aux_skeleton[i],
                             facing_angle, position);
    }
  }

  void
  update(Uint32 dt_ms) {
    if (forward != 0.0f || right != 0.0f) {
      anim_bouncing_ms += dt_ms;
    }

    float component_speed = 1.0f;
    if (forward != 0.0f && right != 0.0f) {
       component_speed = std::sqrt(1.0f/2.0f);
    }

    if (forward != 0.0f) {
      float angle = facing_angle + M_PI*(0.5f + (forward+1.0f)/2.0f);
      Float2 delta_pos = Float2(0, component_speed*speed*dt_ms);
      Float2 rot_angle(std::cos(angle), std::sin(angle));
      position += rotate(delta_pos, rot_angle);
    }

    if (right != 0.0f) {
      float angle = facing_angle - right*M_PI*0.5f;
      Float2 delta_pos(component_speed*speed*dt_ms, 0);
      Float2 rot_angle(std::cos(angle), std::sin(angle));
      position += rotate(delta_pos, rot_angle);
    }
  }

private:
  Float2 facing_unit_direction = Float2(1.0f, 0.0f);
  double facing_angle = 0.0;

  Float2 position;
  float speed = 0.0f;
  float forward = 0.0f;
  float right = 0.0f;

  SHR::Image (*images)[NUM_BODY_PIECES];

  Uint32 anim_bouncing_ms = 0;
};

/**
 * Check char_coords.py to see how these numbers were generated.
 */
const Float2
Character::skeleton[NUM_BODY_PIECES] = {
  //head
  Float2(0.0f, 0.0f),
  //shoulder left
  Float2(-41.0f, -18.0f),
  //shoulder right
  Float2(44.0f, -17.0f),
  //torso
  Float2(1.0f, -25.0f),
  //arm left
  Float2(-58.0f, -10.0f),
  //weapon
  Float2(33.0f, 47.0f),
  //arm right
  Float2(40.0f, 32.0f),
};

class Main {
public:
  Main(const char *title, int width, int height)
    : sdl(SDL_INIT_VIDEO),
      win(title, width, height),
      rend(&win, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
      screen(&rend, width, height),
      images {
        SHR::Image(&screen, xIMG::load("head.png")),
        SHR::Image(&screen, xIMG::load("shoulder_left.png")),
        SHR::Image(&screen, xIMG::load("shoulder_right.png")),
        SHR::Image(&screen, xIMG::load("torso.png")),
        SHR::Image(&screen, xIMG::load("arm_left.png")),
        SHR::Image(&screen, xIMG::load("weapon.png")),
        SHR::Image(&screen, xIMG::load("arm_right.png"))
      },
      character(Float2(0, 0), &images)
  {}

  int
  run() {
    character.set_speed(0.3f);
    Uint32 last_update = SDL_GetTicks();
    for (;;) {
      Uint32 now = SDL_GetTicks();
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          return 0;
        }
        consume_event(event);
      }
      rend.set_draw_color(xSDL::BLACK);
      rend.clear();
      update_and_render(now-last_update);
      last_update = now;
      rend.present();
    }
  }

private:
  void
  update_and_render(Uint32 dt_ms) {
    character.update(dt_ms);
    character.render(&screen);
  }

  void
  consume_event(SDL_Event &e) {
    switch (e.type) {
      case SDL_KEYDOWN:
        if (e.key.repeat) {
          break;
        }
        switch (e.key.keysym.sym) {
          case SDLK_a:
            character.walk_sideway_left();
            break;
          case SDLK_s:
            character.walk_backward();
            break;
          case SDLK_d:
            character.walk_sideway_right();
            break;
          case SDLK_w:
            character.walk_forward();
            break;
        }
        break;
      case SDL_KEYUP:
        if (e.key.repeat) {
          break;
        }
        switch (e.key.keysym.sym) {
          case SDLK_a:
            character.stop_sideway_left();
            break;
          case SDLK_s:
            character.stop_backward();
            break;
          case SDLK_d:
            character.stop_sideway_right();
            break;
          case SDLK_w:
            character.stop_forward();
            break;
        }
        break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        int x, y;
        SDL_GetMouseState(&x, &y);
        y = screen.height() - 1 - y;
        character.face(Float2(x, y));
        break;
      }
    }
  }

private:
  xSDL::SDL sdl;
  xSDL::Window win;
  xSDL::Renderer rend;
  SHR::Screen screen;
  SHR::Image images[Character::NUM_BODY_PIECES];
  Character character;
};

int
main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  try {
    return Main("Walking Character", 800, 600).run();
  }
  catch (std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << ".\n";
    return 1;
  }
}
