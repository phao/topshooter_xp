#include <string>
#include <stdexcept>

#include "xsdl.hpp"

namespace xsdl {

////
// SDL

sdl::
sdl(int flags) {
  if (SDL_Init(flags) < 0) {
    throw sdl_init_error(SDL_GetError());
  }
}

sdl::
~sdl() {
  SDL_Quit();
}

////
// surface

int
surface::
width() const noexcept {
  return surf->w;
}

int
surface::
height() const noexcept {
  return surf->h;
}

////
// renderer

renderer::
renderer(window *win, int flags)
  : rend {SDL_CreateRenderer(win->win, -1, flags)}
{
  if (!rend) {
    throw resource_create_error(SDL_GetError());
  }
}

void
renderer::
copy(texture *texture,
     const rect* src, const rect* dest,
     double angle, const point *center_rot,
     renderer_flip flip)
{
  if (SDL_RenderCopyEx(rend, texture->tex, src, dest, angle, center_rot,
                       flip) < 0)
  {
    throw render_error(SDL_GetError());
  }
}

void
renderer::
fill_rectangle(const rect& rect) {
  if (SDL_RenderFillRect(rend, &rect) < 0) {
    throw render_error(SDL_GetError());
  }
}

void
renderer::
present() noexcept {
  SDL_RenderPresent(rend);
}

void
renderer::
clear() {
  if (SDL_RenderClear(rend) < 0) {
    throw render_error(SDL_GetError());
  }
}

void
renderer::
set_draw_color(color c) {
  if (SDL_SetRenderDrawColor(rend, c.r, c.g, c.b, c.a) < 0) {
    throw resource_alter_error(SDL_GetError());
  }
}

////
// texture

texture::
texture(renderer *rend, surface *surf)
  : tex {SDL_CreateTextureFromSurface(rend->rend, surf->surf)}
{
  if (!tex) {
    throw resource_create_error(SDL_GetError());
  }
}

////
// window

window::
window(const char *title, int x_pos, int y_pos, int width, int height,
       int flags)
  : win {SDL_CreateWindow(title, x_pos, y_pos, width, height, flags)}
{
  if (!win) {
    throw resource_create_error(SDL_GetError());
  }
}

window::
window(const char *title, int width, int height, int flags)
  : window {title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, flags}
{}

} // xsdl
