#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "xsdl.hpp"
#include "graphical.hpp"

namespace GAME {

namespace ATLAS {

enum PieceId {
  HEAD,
  SHOULDER_LEFT,
  SHOULDER_RIGHT,
  TORSO,
  ARM_LEFT,
  WEAPON,
  ARM_RIGHT,
  CIRCLE_GRAD,
  EXPLOSIONS,
  NUM_ATLAS_PIECES
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
