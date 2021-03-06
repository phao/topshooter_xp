#include "Atlas.hpp"
#include "xSDL.hpp"
#include "Graphical.hpp"

namespace GAME {

namespace ATLAS {

struct PieceInfo {
  xSDL::Rect geom;
  int id;
};

static const PieceInfo
pieces_info[] = {
  {{58, 150, 57, 58}, ENG_HEAD},
  {{158, 184, 72, 78}, ENG_SHOULDER_LEFT},
  {{158, 101, 61, 83}, ENG_SHOULDER_RIGHT},
  {{158, 0, 102, 101}, ENG_TORSO},
  {{80, 0, 78, 117}, ENG_ARM_LEFT},
  {{0, 0, 80, 150}, ENG_WEAPON},
  {{0, 150, 58, 114}, ENG_ARM_RIGHT},
  {{219, 133, 18, 18}, CIRCLE_GRAD},
  {{219, 101, 32, 32}, EXPLOSIONS},
};

const xSDL::Rect&
piece_geom(int which) {
  return pieces_info[which].geom;
}

}

}
