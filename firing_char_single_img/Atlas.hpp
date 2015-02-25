#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "xSDL.hpp"
#include "Graphical.hpp"

namespace GAME {

namespace ATLAS {

/**
 * These values are indices into pieces_info. They refer to specific pieces of
 * the atlas.
 */
enum {
  ENG_HEAD,
  ENG_SHOULDER_LEFT,
  ENG_SHOULDER_RIGHT,
  ENG_TORSO,
  ENG_ARM_LEFT,
  ENG_WEAPON,
  ENG_ARM_RIGHT,
  CIRCLE_GRAD,
  EXPLOSIONS,
  NUM_ATLAS_PIECES
};

/**
 * These values talk about pieces of the atlas or collections thereof. They
 * shouldn't be in the enumeration above because that is left solely for
 * identifiers that map directly into pieces_info entries.
 */
enum {
  ENG_PIECES_BEGIN = ENG_HEAD,
  ENG_PIECES_END = ENG_ARM_RIGHT + 1,
  ENG_NUM_PIECES = ENG_PIECES_END - ENG_PIECES_BEGIN
};

const xSDL::Rect&
piece_geom(int which);

}

}

#endif
