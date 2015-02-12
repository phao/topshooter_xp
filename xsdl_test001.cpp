#include "xsdl.hpp"

int
main() {
  xsdl::sdl(SDL_INIT_VIDEO);
  xsdl::window win("Test xSDL", 800, 600);
  constexpr int render_flags = SDL_RENDERER_ACCELERATED |
                               SDL_RENDERER_PRESENTVSYNC;
  xsdl::renderer rend(&win, render_flags);
}
