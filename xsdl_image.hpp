#ifndef X_SDL_IMAGE_HPP
#define X_SDL_IMAGE_HPP

#include "xsdl.hpp"

#include <SDL2/SDL_image.h>

namespace ximg {

xsdl::surface
load(const char *file_name) {
  SDL_Surface *surf = IMG_Load(file_name);
  if (!surf) {
    throw xsdl::io_load_error(IMG_GetError());
  }
  return xsdl::surface(surf);
}

} // ximg

#endif
