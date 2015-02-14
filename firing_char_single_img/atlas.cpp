#include "atlas.hpp"
#include "xsdl.hpp"
#include "graphical.hpp"

namespace GAME {

namespace ATLAS {

const PieceInfo
pieces_info[] = {
  {{58, 150, 57, 58}, HEAD},
  {{158, 184, 72, 78}, SHOULDER_LEFT},
  {{158, 101, 61, 83}, SHOULDER_RIGHT},
  {{158, 0, 102, 101}, TORSO},
  {{80, 0, 78, 117}, ARM_LEFT},
  {{0, 0, 80, 150}, WEAPON},
  {{0, 150, 58, 114}, ARM_RIGHT},
  {{219, 133, 18, 18}, CIRCLE_GRAD},
  {{219, 101, 32, 32}, EXPLOSIONS},
};

GRAL::Image
extract_image(GRAL::Screen *screen,
              xSDL::Surface *atlas,
              PieceId which)
{
  return GRAL::Image{
    screen,
    xSDL::Surface{atlas, pieces_info[which].geom}
  };
}

}

}
