#ifndef SHARED_H
#define SHARED_H

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

#include <math.h>

#include <SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SHR_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SHR_MAX(a, b) ((a) > (b) ? (a) : (b))

struct SHR_Float2 {
  float x, y;
};

struct SHR_Image {
  SDL_Texture *tex;
  int w, h;
};

struct SHR_Screen {
  SDL_Renderer *rend;
  int w, h;
};

inline void
SHR_ScreenSetup(struct SHR_Screen *screen,
                SDL_Renderer *rend,
                int w, int h)
{
  screen->rend = rend;
  screen->w = w;
  screen->h = h;
}

int
SHR_LoadImage(SDL_Renderer *rend,
              const char *file_name,
              struct SHR_Image *out_img);

void
SHR_DestroyImage(struct SHR_Image *img);

/**
 * Draw the image with its CENTER at x,y and rotated by ANGLE around its
 * center. You should also specify x,y from left->right and bottom->up.
 *
 * @note The flip is as in SDL_RenderCopyEx.
 *
 * @note Angle is in radians.
 *
 * @note Generally speaking, the rotation is independent of the position, which
 * is independent of the flip. The parameters are chosen so that no matter
 * the order in which you think of applying these transformations (translation,
 * rotation, flip), you get the same results. This obviously restricts what
 * you can do, but it also makes it way simpler to use this function.
 */
int
SHR_DrawRightImage(struct SHR_Screen *screen,
                   const struct SHR_Image *img,
                   struct SHR_Float2 center,
                   float angle,
                   SDL_RendererFlip flip);

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points up (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI*0.5, flip)
 */
int
SHR_DrawUpImage(struct SHR_Screen *screen,
                const struct SHR_Image *img,
                struct SHR_Float2 center,
                float angle,
                SDL_RendererFlip flip);

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points right (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI, flip)
 */
int
SHR_DrawLeftImage(struct SHR_Screen *screen,
                  const struct SHR_Image *img,
                  struct SHR_Float2 center,
                  float angle,
                  SDL_RendererFlip flip);

/**
 * Directly calls DrawLeftImage, however, it normalizes the angle to correct
 * for the fact that the image points down (and not left) by default.
 *
 * Basically, it's equivalent to calling:
 * SHR_DrawRightImage(img, x, y, angle - M_PI*1.5, flip)
 */
int
SHR_DrawDownImage(struct SHR_Screen *screen,
                  const struct SHR_Image *img,
                  struct SHR_Float2 center,
                  float angle,
                  SDL_RendererFlip flip);

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
struct SHR_Float2
SHR_CenterForBaseAt(const struct SHR_Image *img,
                    struct SHR_Float2 base,
                    struct SHR_Float2 unit_direction);

/**
 * Assumes the base of the image is at its mid-left point. This isn't a problem
 * if you're drawing your images with the appropriated Draw<DIR>Image call.
 */
struct SHR_Float2
SHR_CenterForTopAt(const struct SHR_Image *img,
                   struct SHR_Float2 top,
                   struct SHR_Float2 unit_direction);

inline float
SHR_Dot_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b) {
  return float2_a.x*float2_b.x + float2_a.y*float2_b.y;
}

inline float
SHR_Norm2_f2(struct SHR_Float2 float2) {
  return SHR_Dot_f2(float2, float2);
}

inline float
SHR_Norm_f2(struct SHR_Float2 float2) {
  return sqrtf(SHR_Norm2_f2(float2));
}

inline struct SHR_Float2
SHR_Sub_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b) {
  return (struct SHR_Float2) {float2_a.x-float2_b.x, float2_a.y-float2_b.y};
}

inline struct SHR_Float2
SHR_Add_f2(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b) {
  return (struct SHR_Float2) {float2_a.x + float2_b.x, float2_a.y + float2_b.y};
}

inline struct SHR_Float2
SHR_Scale_f2(float x, struct SHR_Float2 float2) {
  return (struct SHR_Float2) {x*float2.x, x*float2.y};
}

inline struct SHR_Float2
SHR_Normalize_f2(struct SHR_Float2 float2) {
  return SHR_Scale_f2(1.0f/SHR_Norm_f2(float2), float2);
}

inline int
SHR_EpsEq(struct SHR_Float2 float2_a, struct SHR_Float2 float2_b, float eps) {
  // Is this a good way to implement 'equality'?
  float abs_dx = fabs(float2_a.x-float2_b.x);
  float abs_dy = fabs(float2_a.y-float2_b.y);
  return abs_dx < eps && abs_dy < eps;
}

inline struct SHR_Float2
SHR_Make_f2(float x, float y) {
  return (struct SHR_Float2){x, y};
}

inline void
SHR_ScaleInto_f2(struct SHR_Float2 *float2, float f) {
  float2->x *= f;
  float2->y *= f;
}

#endif
