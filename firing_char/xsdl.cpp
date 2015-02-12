#include <string>
#include <stdexcept>

#include "xsdl.hpp"

namespace xsdl {

////
// SDL

SDL::
SDL(int flags) {
  if (SDL_Init(flags) < 0) {
    throw SDLInitError(SDL_GetError());
  }
}

SDL::
~SDL() {
  SDL_Quit();
}

////
// surface

int
Surface::
width() const noexcept {
  return surf->w;
}

int
Surface::
height() const noexcept {
  return surf->h;
}

////
// renderer

Renderer::
Renderer(Window *win, int flags)
  : rend {SDL_CreateRenderer(win->win, -1, flags)}
{
  if (!rend) {
    throw ResourceCreateError(SDL_GetError());
  }
}

void
Renderer::
copy(Texture *texture,
     const Rect* src, const Rect* dest,
     double angle, const Point *center_rot,
     RenderFlip flip)
{
  if (SDL_RenderCopyEx(rend, texture->tex, src, dest, angle, center_rot,
                       flip) < 0)
  {
    throw RenderError(SDL_GetError());
  }
}

void
Renderer::
fill_rectangle(const Rect& rect) {
  if (SDL_RenderFillRect(rend, &rect) < 0) {
    throw RenderError(SDL_GetError());
  }
}

void
Renderer::
present() noexcept {
  SDL_RenderPresent(rend);
}

void
Renderer::
clear() {
  if (SDL_RenderClear(rend) < 0) {
    throw RenderError(SDL_GetError());
  }
}

void
Renderer::
set_draw_color(Color c) {
  if (SDL_SetRenderDrawColor(rend, c.r, c.g, c.b, c.a) < 0) {
    throw ResourceAlterError(SDL_GetError());
  }
}

////
// Texture

Texture::
Texture(Renderer *rend, Surface *surf)
  : tex {SDL_CreateTextureFromSurface(rend->rend, surf->surf)}
{
  if (!tex) {
    throw ResourceCreateError(SDL_GetError());
  }
}

void
Texture::
set_alpha_mod(uint8_t alpha_mod) {
  if (SDL_SetTextureAlphaMod(tex, alpha_mod) < 0) {
    throw ResourceAlterError(SDL_GetError());
  }
}

void
Texture::
set_color_mod(Color color_mod) {
  if (SDL_SetTextureColorMod(tex, color_mod.r, color_mod.g, color_mod.b) < 0) {
    throw ResourceAlterError(SDL_GetError());
  }
}

void
Texture::
set_blend_mode(BlendMode mode) {
  if (SDL_SetTextureBlendMode(tex, mode) < 0) {
    throw ResourceAlterError(SDL_GetError());
  }
}

uint8_t
Texture::
get_alpha_mod() const {
  uint8_t mod;
  if (SDL_GetTextureAlphaMod(tex, &mod) < 0) {
    throw ResourceReadError(SDL_GetError());
  }
  return mod;
}

Color
Texture::
get_color_mod() const {
  uint8_t r, g, b;
  if (SDL_GetTextureColorMod(tex, &r, &g, &b) < 0) {
    throw ResourceReadError(SDL_GetError());
  }
  return {r, g, b};
}

BlendMode
Texture::
get_blend_mode() const {
  BlendMode mode;
  if (SDL_GetTextureBlendMode(tex, &mode) < 0) {
    throw ResourceReadError(SDL_GetError());
  }
  return mode;
}

////
// window

Window::
Window(const char *title, int x_pos, int y_pos, int width, int height,
       int flags)
  : win {SDL_CreateWindow(title, x_pos, y_pos, width, height, flags)}
{
  if (!win) {
    throw ResourceCreateError(SDL_GetError());
  }
}

Window::
Window(const char *title, int width, int height, int flags)
  : Window {title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, flags}
{}

} // xsdl
