#ifndef X_SDL_IMAGE_HPP
#define X_SDL_IMAGE_HPP

#include "xSDL.hpp"

#include <SDL2/SDL_image.h>

namespace xIMG {

xSDL::Surface
load(const char *file_name) {
  SDL_Surface *surf = IMG_Load(file_name);
  if (!surf) {
    throw xSDL::LoadError(IMG_GetError());
  }
  return xSDL::Surface(surf);
}


} // xIMG

#endif
