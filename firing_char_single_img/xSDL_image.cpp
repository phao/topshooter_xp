#include "xSDL.hpp"

#include <SDL2/SDL_image.h>

namespace xIMG {

xSDL::Surface
load(const char *file_name) {
  SDL_Surface *surf = IMG_Load(file_name);
  if (!surf) {
    throw xSDL::IOLoadError(IMG_GetError());
  }
  return xSDL::Surface(surf);
}

} // ximg
