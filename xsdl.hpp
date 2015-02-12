#ifndef X_SDL_HPP
#define X_SDL_HPP

#include <cassert>
#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

#define X_SDL_RESOURCE_COMMON_BODY(type, underlying_ptr_type, \
                                   underlying_ptr_var, del_func) \
  public: \
  explicit type(underlying_ptr_type underlying_ptr_var) noexcept \
    : underlying_ptr_var {underlying_ptr_var} \
  { SDL_assert(underlying_ptr_var); } \
  type& operator = (const type &) = delete; \
  type(const type &) = delete; \
  type& operator = (type&& src) noexcept { \
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

class texture;
class window;

////
// load_error

class io_load_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// render_error

class render_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// sdl_init_error

class sdl_init_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// resource_create_error

class resource_create_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// resource_alter_error

class resource_alter_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

////
// color

struct color : public SDL_Color {
  color() noexcept
  {}

  constexpr
  color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
    : SDL_Color {r, g, b, a}
  {}

  constexpr
  color(uint8_t gray_level) noexcept
    : color {gray_level, gray_level, gray_level, gray_level}
  {}
};

static constexpr color BLACK {0, 0, 0};
static constexpr color WHITE {255, 255, 255};
static constexpr color RED {255, 0, 0};
static constexpr color GREEN {0, 255, 0};
static constexpr color BLUE {0, 0, 255};
static constexpr color YELLOW {255, 255, 0};
static constexpr color CYAN {0, 255, 255};

////
// rect

struct rect : public SDL_Rect {
  constexpr
  rect(int x, int y, int w, int h) noexcept
    : SDL_Rect {x, y, w, h}
  {}

  rect() noexcept
  {}
};

////
// point

struct point : public SDL_Point {
  constexpr
  point(int x, int y) noexcept
    : SDL_Point {x, y}
  {}

  point() noexcept
  {}
};

////
// sdl

struct sdl {
  explicit sdl(int flags);

  // No copy constructor or assignment operator.
  sdl& operator = (const sdl&) = delete;
  sdl(const sdl&) = delete;

  // Default move assignment and move constructors will do.
  sdl& operator = (sdl&&) = default;
  sdl(sdl&&) = default;

  ~sdl();
};

////
// surface

class surface {
  friend class texture;

  X_SDL_RESOURCE_COMMON_BODY(surface, SDL_Surface*, surf, SDL_FreeSurface)

public:
  int
  width() const noexcept;

  int
  height() const noexcept;
};

/////
// renderer

using renderer_flip = SDL_RendererFlip;

class renderer {
  friend class texture;

  X_SDL_RESOURCE_COMMON_BODY(renderer, SDL_Renderer*, rend,
                             SDL_DestroyRenderer)

public:
  renderer(window *win, int flags);

  void
  copy(texture *texture,
       const rect* src, const rect* dest,
       double angle, const point *center_rot,
       renderer_flip flip = SDL_FLIP_NONE);

  void
  present() noexcept;

  void
  clear();

  void
  set_draw_color(color c);

  void
  fill_rectangle(const rect& rect);
};

////
// texture

class texture {
  friend class renderer;

  X_SDL_RESOURCE_COMMON_BODY(texture, SDL_Texture*, tex,
                             SDL_DestroyTexture)

public:
  texture(renderer *rend, surface *surf);
};

////
// window

class window {
  friend class renderer;

  X_SDL_RESOURCE_COMMON_BODY(window, SDL_Window*, win, SDL_DestroyWindow)

public:
  window(const char *title, int x_pos, int y_pos, int width, int height,
         int flags = SDL_WINDOW_SHOWN);

  window(const char *title, int width, int height,
         int flags = SDL_WINDOW_SHOWN);
};

} // xSDL

#endif
