#include <SDL2/SDL.h>

#include "Atlas.h"
#include "Graphical.h"

static const SDL_Rect
pieces_info[] = {
  {58, 150, 57, 58},
  {158, 184, 72, 78},
  {158, 101, 61, 83},
  {158, 0, 102, 101},
  {80, 0, 78, 117},
  {0, 0, 80, 150},
  {0, 150, 58, 114},
  {219, 133, 18, 18},
  {219, 101, 32, 32}
};

int
ATLAS_SetupImage(GRAL_Screen *screen,
                 GRAL_Image *img,
                 SDL_Surface *atlas,
                 int which)
{
  return GRAL_SetupImageFromSurfaceRegion(screen, img, atlas,
                                          pieces_info+which);
}
