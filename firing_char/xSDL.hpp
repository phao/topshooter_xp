#ifndef X_SDL_HPP
#define X_SDL_HPP

#include <cassert>
#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

#define X_SDL_RESOURCE_COMMON_BODY(type, underlying_type, underlying_var, \
                                   del_func) \
  public: \
  explicit type(underlying_type underlying_var) noexcept \
    : underlying_var(underlying_var) \
  { SDL_assert(underlying_var); } \
  type& operator = (const type &) = delete; \
  type(const type &) = delete; \
  type& operator = (type&& src) noexcept { \
    if (src.underlying_var != underlying_var) { \
      del_func(underlying_var); \
      underlying_var = src.underlying_var; \
    } \
    src.underlying_var = nullptr; \
    return *this; \
  } \
  type(type&& src) noexcept { \
    underlying_var = src.underlying_var; \
    src.underlying_var = nullptr; \
  } \
  ~type() noexcept { \
    if (underlying_var) { \
      del_func(underlying_var); \
    } \
  } \
  private: \
  underlying_type underlying_var;


namespace xSDL {

class Texture;
class Window;

////
// LoadError

class LoadError : public std::runtime_error {
public:
  explicit LoadError(const std::string& what_arg)
    : std::runtime_error(what_arg)
  {}

  explicit LoadError(const char* what_arg)
    : std::runtime_error(what_arg)
  {}
};

////
// Color

struct Color {
  Uint8 colors[4];

  Color()
  {}

  constexpr
  Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255)
    : colors {red, green, blue, alpha}
  {}

  constexpr
  Color(Uint8 gray_level)
    : Color {gray_level, gray_level, gray_level, gray_level}
  {}

  enum ColorType {
    RGB = 3,
    RGBA = 4
  };

  template<ColorType color_type>
  Color(const Uint8 *colors) {
    static_assert(color_type == RGB || color_type == RGBA,
                  "Invalid color type (must be RGB or RGBA).");
    memcpy(this->colors, colors, color_type);
  }

  Uint8&
  red() {
    return colors[0];
  }

  Uint8&
  blue() {
    return colors[1];
  }

  Uint8&
  green() {
    return colors[2];
  }

  Uint8&
  alpha() {
    return colors[3];
  }

  constexpr Uint8
  red() const {
    return colors[0];
  }

  constexpr Uint8
  blue() const {
    return colors[1];
  }

  constexpr Uint8
  green() const {
    return colors[2];
  }

  constexpr Uint8
  alpha() const {
    return colors[3];
  }
};

static constexpr Color BLACK {0, 0, 0};

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
  width() const;

  int
  height() const;
};

////
// Renderer

class Renderer {
  friend class Texture;

  X_SDL_RESOURCE_COMMON_BODY(Renderer, SDL_Renderer*, rend, SDL_DestroyRenderer)

public:
  Renderer(Window *win, int flags);

  void
  copy(Texture *texture,
       const SDL_Rect* src, const SDL_Rect* dest,
       double angle, const SDL_Point *center_rot,
       SDL_RendererFlip flip = SDL_FLIP_NONE);

  void
  present();

  void
  clear();

  void
  set_draw_color(Color c);

  void
  fill_rectangle(const SDL_Rect& rect);
};

////
// Texture

class Texture {
  friend class Renderer;

  X_SDL_RESOURCE_COMMON_BODY(Texture, SDL_Texture*, tex, SDL_DestroyTexture)

public:
  Texture(Renderer *rend, Surface *surf);
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

} // xSDL

#endif
