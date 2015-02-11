#include <string>
#include <stdexcept>

#include "xSDL.hpp"

namespace xSDL {

////
// SDL

SDL::
SDL(int flags) {
  if (SDL_Init(flags) < 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

SDL::
~SDL() {
  SDL_Quit();
}

////
// Surface

int
Surface::
width() const {
  return surf->w;
}

int
Surface::
height() const {
  return surf->h;
}

////
// Renderer

Renderer::
Renderer(Window *win, int flags)
  : rend(SDL_CreateRenderer(win->win, -1, flags))
{
  if (!rend) {
    throw std::runtime_error(SDL_GetError());
  }
}

void
Renderer::
copy(Texture *texture,
     const SDL_Rect* src, const SDL_Rect* dest,
     double angle, const SDL_Point *center_rot,
     SDL_RendererFlip flip)
{
  if (SDL_RenderCopyEx(rend, texture->tex, src, dest, angle, center_rot,
                       flip) < 0)
  {
    throw std::runtime_error(SDL_GetError());
  }
}

void
Renderer::
fill_rectangle(const SDL_Rect& rect) {
  if (SDL_RenderFillRect(rend, &rect) < 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

void
Renderer::
present() {
  SDL_RenderPresent(rend);
}

void
Renderer::
clear() {
  if (SDL_RenderClear(rend) < 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

void
Renderer::
set_draw_color(Color c) {
  if (SDL_SetRenderDrawColor(rend, c.red(), c.green(), c.blue(),c.alpha()) < 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

////
// Texture

Texture::
Texture(Renderer *rend, Surface *surf)
  : tex(SDL_CreateTextureFromSurface(rend->rend, surf->surf))
{
  if (!tex) {
    throw std::runtime_error(SDL_GetError());
  }
}

////
// Window

Window::
Window(const char *title, int x_pos, int y_pos, int width, int height,
       int flags)
  : win(SDL_CreateWindow(title, x_pos, y_pos, width, height, flags))
{
  if (!win) {
    throw std::runtime_error(SDL_GetError());
  }
}

Window::
Window(const char *title, int width, int height, int flags)
  : Window(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
           width, height, flags)
{}

} // xSDL
