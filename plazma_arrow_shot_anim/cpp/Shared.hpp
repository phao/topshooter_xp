#ifndef SHARED_H
#define SHARED_H

#include <cmath>

#include <SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SHR {

/**
 * The idea here is to attempt for the first time to put "common things
 * together" for the topshooter code.
 *
 * As of now, it contains basic type definitions, and also drawing and
 * positioning functions. Some important assumptions are also baked into
 * here:
 *
 * - Things should be in floating point values (float) as much as possible.
 *
 * - Images are assumed to be pointing right with their base point at
 * their mid-left and their top point at their mid-right. This implies for
 * example that their horizontal extent is actually their height! It also
 * implies that their vertical extent is actually their width!
 *
 * - Drawing an image involves specifying a rotation around its center, and
 * the position of its center in the destination screen (it also involves
 * specifying more things, but these 2 are the most important).
 *
 * - The drawing functions will always rotate images around their center.
 * There are no exceptions to this.
 *
 * - You should stick to CenterFor<Base|Top>At functions for calculating
 * positions of where to draw images.
 *
 * - Draw<DIR>Image assumes the following coordinate system: bottom-left screen
 * point is the origin; y grows up; x grows right.
 *
 * - If your image doesn't point right, you should manipulate it as if it
 * did and then call the right drawing function when drawing (e.g. if your
 * image points down, you should call DrawDownImage). If you have an image
 * which naturally points at 55 degrees, what you're going to do is treat it
 * as if it pointed right and you'll draw it with something like
 * DrawLeftImage(..., -55 degrees in radians, ...).
 *
 * Treating images this way can be quite confusing if you're doing all the
 * coordinates transformations yourself, so you should try to as much as
 * possible think in terms of Draw<DIR>Image and CenterFor<Base|Top>At. While
 * doing *your own* manipulations, if you stick to the assumption that images
 * point right and so forth, things are actually very straightforward. The
 * complication comes in if you try to bypass Draw<DIR>Image and/or
 * CenterFor<Base|Top>At.
 *
 * Once you know the rotation (around its center) the image should have and
 * its base (or top) point, you should call CenterFor<Base|Top>At to get
 * the values for the center point. Then you should call Draw<DIR>Image for
 * drawing.
 *
 * CenterFor<Base|Top>At is basically your positioning routine. With the
 * given scheme for dealing with images, it's quite difficult to figure out
 * by head or even on paper where everything should go. Thinking in terms of
 * (1) base points for images, (2) top points for images, (3) images pointing
 * right, and (4) coordinate systems where y-positive is up makes things
 * extremely easier. This happens because, then, given you draw your image
 * correctly (i.e. with the right Draw<DIR>Image call), all you need to
 * know are things like:
 *
 * - What is the center point (which Draw<DIR>Image takes) so that the base or
 * top point is <such> for a center rotation of <so much>?
 *
 * This question is basically what CenterFor<Base|Top>At answers. It lets you
 * give it the base or top point you'd like to have, the rotation you'd like
 * to have for the image, and it spills you out the center point you should
 * pass to Draw<DIR>Image together with the rotation value so the image gets
 * drawn as you'd expect.
 */

struct Float2 {
  float x, y;

  Float2(x, y) noexcept
    : x(x), y(y)
  {}

  Float2(x) noexcept
    : x(x), y(x)
  {}

  Float2() noexcept
  {}
};

class Image {
  public:
    Image(SDL_Texture *tex, int w, int h) noexcept
      : tex(tex), w(w), h(h)
    {}

    Image(const Image& from) = delete;
    Image& operator = (const Image& from) = delete;

    Image(const Image&& from) = default;
    Image&& operator = (const Image&& src) = default;

    int
    w() const {
      return w;
    }

    int
    h() const {
      return w;
    }

    SDL_Texture *
    texture() {
      return tex;
    }



  private:
    SDL_Texture *tex;
    int w, h;
};

class Screen {
  public:
    Screen(SDL_Renderer *rend, int w, int h) noexcept
      : rend(rend), w(w), h(h)
    {}

    Screen(const Screen& from) = delete;
    Screen& operator = (const Screen& from) = delete;

    Screen(const Screen&& from) = default;
    Screen&& operator = (const Screen&& src) = default;

    int
    w() const {
      return w;
    }

    int
    h() const {
      return w;
    }

    Image
    LoadImage(const char *file_name) {
      SDL_Surface *surf = IMG_Load(file_name);

      if (!surf) {
        throw ImageLoadException("%s: %s", file_name, IMG_GetError());
      }

      SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surf);

      Image img(tex, surf->w, surf->h);

      SDL_FreeSurface(surf);

      if (!tex) {
        throw CreateTextureException(SDL_GetError());
      }

      return img;
    }

    /**
     * Draw the image with its CENTER at x,y and rotated by ANGLE around its
     * center. You should also specify x,y from left->right and bottom->up.
     *
     * @note The flip is as in SDL_RenderCopyEx.
     *
     * @note Angle is in radians.
     *
     * @note Generally speaking, the rotation is independent of the position,
     * which is independent of the flip. The parameters are chosen so that no
     * matter the order in which you think of applying these transformations
     * (translation, rotation, flip), you get the same results. This obviously
     * restricts what you can do, but it also makes it way simpler to use this
     * function.
     */
    void
    DrawRightImage(const Image &img,
                   Float2 center,
                   float angle,
                   SDL_RendererFlip flip)
    {
      const double RAD_TO_DEG = 180.0 / M_PI;

      float x, y;

      y = (h - 1 - center.y) - img.h/2;
      x = center.x - img.w/2;
      angle *= -1;
      SDL_Rect dest_rect = {x, y, img.w, img.h};
      int render_status = SDL_RenderCopyEx(rend, img.texture(),
                                           nullptr, &dest_rect,
                                           angle*RAD_TO_DEG, nullptr,
                                           flip);
      if (render_status < 0) {
        throw RenderImageException(SDL_GetErro());
      }
    }

    /**
     * Directly calls DrawLeftImage, however, it normalizes the angle to
     * correct for the fact that the image points up (and not left) by
     * default.
     *
     * Basically, it's equivalent to calling:
     * DrawRightImage(img, x, y, angle - M_PI*0.5, flip)
     */
    void
    DrawUpImage(const Image &img,
                Float2 center,
                float angle,
                SDL_RendererFlip flip)
    {
      return DrawRightImage(img, center, angle - M_PI*0.5, flip);
    }

    /**
     * Directly calls DrawLeftImage, however, it normalizes the angle to
     * correct for the fact that the image points right (and not left) by
     * default.
     *
     * Basically, it's equivalent to calling:
     * DrawRightImage(img, x, y, angle - M_PI, flip)
     */
    void
    DrawLeftImage(const Image &img,
                  Float2 center,
                  float angle,
                  SDL_RendererFlip flip)
    {
      return DrawRightImage(img, center, angle - M_PI, flip);
    }

    /**
     * Directly calls DrawLeftImage, however, it normalizes the angle to
     * correct for the fact that the image points down (and not left) by default.
     *
     * Basically, it's equivalent to calling:
     * DrawRightImage(img, x, y, angle - M_PI*1.5, flip)
     */
    void
    DrawDownImage(const Image &img,
                  Float2 center,
                  float angle,
                  SDL_RendererFlip flip)
    {
      return DrawRightImage(screen, img, center, angle - M_PI*1.5, flip);
    }

  private:
    SDL_Renderer *rend;
    int w, h;
};

inline float
Dot(Float2 a, Float2 b) {
  return a.x*b.x + a.y*b.y;
}

inline float
Norm2(Float2 a) {
  return Dot(a, a);
}

inline float
Norm(Float2 a) {
  return std::sqrt(Norm2(a));
}

inline Float2
operator - (Float2 a, Float2 b) {
  return Float2(a.x - b.x, a.y - b.y);
}

inline Float2
operator + (Float2 a, Float2 b) {
  return Float2(a.x + b.x, a.y + b.y};
}

inline Float2
operator * (float x, Float2 a) {
  return Float2(x*a.x, x*a.y);
}

inline Float2
operator * (Float2 a, float x) {
  return x*a;
}

inline Float2
operator - (Float2 a) {
  return Float2(-a.x, -a.y);
}

inline Float2
Normalize(Float2 a) {
  return 1.0f/Norm(a) * a;
}

inline bool
EpsEq(Float2 a, Float2 b, float eps) {
  // Is this a good way to implement 'equality'?
  float abs_dx = std::abs(a.x-b.x);
  float abs_dy = std::abs(a.y-b.y);
  return abs_dx < eps && abs_dy < eps;
}

} // end of SHR namespace

#endif
