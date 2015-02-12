#ifndef X_SDL_HPP
#define X_SDL_HPP

#include <iostream>

#include <cassert>
#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

#define X_SDL_RESOURCE_COMMON_BODY(type, underlying_ptr_type, \
                                   underlying_ptr_var, \
                                   del_func) \
  public: \
  explicit type(underlying_ptr_type underlying_ptr_var) noexcept \
    : underlying_ptr_var {underlying_ptr_var} \
  { SDL_assert(underlying_ptr_var); } \
  type& operator=(const type &) = delete; \
  type(const type &) = delete; \
  type& operator=(type&& src) noexcept { \
    /* Self move assign? */ \
    if (src.underlying_ptr_var != underlying_ptr_var) { \
      del_func(underlying_ptr_var); \
      underlying_ptr_var = src.underlying_ptr_var; \
      src.underlying_ptr_var = nullptr; \
    } \
    return *this; \
  } \
  type(type&& src) noexcept { \
    underlying_ptr_var = src.underlying_ptr_var; \
    src.underlying_ptr_var = nullptr; \
  } \
  ~type() noexcept { \
    if (underlying_ptr_var) { \
      del_func(underlying_ptr_var); \
      underlying_ptr_var = nullptr; \
    } \
  } \
  private: \
  underlying_ptr_type underlying_ptr_var;


namespace xsdl {

class Texture;
class Window;

////
// Error

class Error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// IOLoadError

class IOLoadError : public Error {
public:
  using Error::Error;
};

////
// RenderError

class RenderError : public Error {
public:
  using Error::Error;
};

////
// SDLInitError

class SDLInitError : public Error {
public:
  using Error::Error;
};

////
// ResourceCreateError

class ResourceCreateError : public Error {
public:
  using Error::Error;
};

////
// ResourceAlterError

class ResourceAlterError : public Error {
public:
  using Error::Error;
};

////
// ResourceReadError

class ResourceReadError : public Error {
public:
  using Error::Error;
};

////
// Color

struct Color : public SDL_Color {
  Color() noexcept
  {}

  constexpr
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
    : SDL_Color {r, g, b, a}
  {}

  constexpr
  Color(uint8_t gray_level) noexcept
    : Color {gray_level, gray_level, gray_level, gray_level}
  {}

  constexpr
  Color(const SDL_Color &c) noexcept
    : Color(c.r, c.g, c.b, c.a)
  {}
};

static constexpr Color BLACK {0, 0, 0};
static constexpr Color WHITE {255, 255, 255};
static constexpr Color RED {255, 0, 0};
static constexpr Color GREEN {0, 255, 0};
static constexpr Color BLUE {0, 0, 255};
static constexpr Color YELLOW {255, 255, 0};
static constexpr Color CYAN {0, 255, 255};

////
// Rect

struct Rect : public SDL_Rect {
  constexpr
  Rect(int x, int y, int w, int h) noexcept
    : SDL_Rect {x, y, w, h}
  {}

  Rect() noexcept
  {}

  constexpr
  Rect(const SDL_Rect &r) noexcept
    : Rect(r.x, r.y, r.w, r.h)
  {}
};

////
// Point

struct Point : public SDL_Point {
  constexpr
  Point(int x, int y) noexcept
    : SDL_Point {x, y}
  {}

  Point() noexcept
  {}

  Point(SDL_Point p) noexcept
    : Point(p.x, p.y)
  {}
};

////
// SDL

struct SDL {
  explicit SDL(int flags);

  // No copy constructor or assignment operator.
  SDL& operator = (const SDL&) = delete;
  SDL(const SDL&) = delete;

  // Default move assignment and move constructors will do.
  SDL& operator = (SDL&&) = default;
  SDL(SDL&&) = default;

  ~SDL();
};

////
// Surface

class Surface {
  friend class Texture;

  X_SDL_RESOURCE_COMMON_BODY(Surface, SDL_Surface*, surf, SDL_FreeSurface)

public:
  int
  width() const noexcept;

  int
  height() const noexcept;
};

/////
// Renderer

using RenderFlip = SDL_RendererFlip;
using BlendMode = SDL_BlendMode;

class Renderer {
  friend class Texture;

  X_SDL_RESOURCE_COMMON_BODY(Renderer, SDL_Renderer*, rend,
                             SDL_DestroyRenderer)

public:
  Renderer(Window *win, int flags);

  void
  copy(Texture *Texture,
       const Rect* src, const Rect* dest,
       double angle, const Point *center_rot,
       RenderFlip flip = SDL_FLIP_NONE);

  void
  present() noexcept;

  void
  clear();

  void
  set_draw_color(Color c);

  void
  fill_rectangle(const Rect& Rect);
};

////
// Texture

class Texture {
  friend class Renderer;

  X_SDL_RESOURCE_COMMON_BODY(Texture, SDL_Texture*, tex,
                             SDL_DestroyTexture)

public:
  Texture(Renderer *rend, Surface *surf);

  void
  set_alpha_mod(uint8_t alpha_mod);

  void
  set_color_mod(Color color_mod);

  void
  set_blend_mode(BlendMode mode);

  uint8_t
  get_alpha_mod() const;

  Color
  get_color_mod() const;

  BlendMode
  get_blend_mode() const;
};

////
// Window

class Window {
  friend class Renderer;

  X_SDL_RESOURCE_COMMON_BODY(Window, SDL_Window*, win, SDL_DestroyWindow)

public:
  Window(const char *title, int x_pos, int y_pos, int width, int height,
         int flags = SDL_WINDOW_SHOWN);

  Window(const char *title, int width, int height,
         int flags = SDL_WINDOW_SHOWN);
};

} // xsdl

#endif
