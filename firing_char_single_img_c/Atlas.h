#ifndef ATLAS_H
#define ATLAS_H

#include <SDL2/SDL.h>

#include "Graphical.h"

/**
 * These values are indices into pieces_info. They refer to specific pieces of
 * the atlas.
 */
enum {
  ATLAS_ENG_HEAD,
  ATLAS_ENG_SHOULDER_LEFT,
  ATLAS_ENG_SHOULDER_RIGHT,
  ATLAS_ENG_TORSO,
  ATLAS_ENG_ARM_LEFT,
  ATLAS_ENG_WEAPON,
  ATLAS_ENG_ARM_RIGHT,
  ATLAS_CIRCLE_GRAD,
  ATLAS_EXPLOSIONS,
  ATLAS_NUM_PIECES
};

/**
 * These values talk about pieces of the atlas or collections thereof. They
 * shouldn't be in the enumeration above because that is left solely for
 * identifiers that map directly into pieces_info entries.
 */
enum {
  ATLAS_ENG_PIECES_BEGIN = ATLAS_ENG_HEAD,
  ATLAS_ENG_PIECES_END = ATLAS_ENG_ARM_RIGHT + 1,
  ATLAS_ENG_NUM_PIECES = ATLAS_ENG_PIECES_END - ATLAS_ENG_PIECES_BEGIN
};

int
ATLAS_SetupImage(GRAL_Screen *screen,
                 GRAL_Image *img,
                 SDL_Surface *atlas,
                 int which);

#endif
