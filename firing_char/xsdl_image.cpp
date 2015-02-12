#include "xsdl.hpp"

#include <SDL2/SDL_image.h>

namespace ximg {

xsdl::Surface
load(const char *file_name) {
  SDL_Surface *surf = IMG_Load(file_name);
  if (!surf) {
    throw xsdl::IOLoadError(IMG_GetError());
  }
  return xsdl::Surface(surf);
}

} // ximg
