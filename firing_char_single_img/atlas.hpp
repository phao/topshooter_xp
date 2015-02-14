#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "xsdl.hpp"
#include "graphical.hpp"

namespace GAME {

namespace ATLAS {

enum PieceId {
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

enum PieceIdMarker {
  ENG_END_PIECES = ENG_ARM_RIGHT + 1,
  ENG_PIECES = ENG_END_PIECES - ENG_HEAD
};

struct PieceInfo {
  xSDL::Rect geom;
  PieceId id;
};

extern const PieceInfo pieces_info[];

GRAL::Image
extract_image(GRAL::Screen *screen,
              xSDL::Surface *atlas,
              PieceId which);

}

}

#endif
