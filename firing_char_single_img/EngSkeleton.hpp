#ifndef ENG_SKELETON_HPP
#define ENG_SKELETON_HPP

#include "Atlas.hpp"
#include "Graphical.hpp"
#include "xSDL.hpp"
#include "StaticBuffer.hpp"

namespace GAME {

class EngSkeleton {
public:
  EngSkeleton(GRAL::Screen *screen, xSDL::Surface *atlas) {
    for (int i = 0; i < ATLAS::ENG_NUM_PIECES; ++i) {
      skeleton_buffer.emplace_back(screen, atlas,
                                   ATLAS::piece_geom(ATLAS::ENG_HEAD + i));
    }
  }

  typedef GRAL::Image (*ArrayPtr)[ATLAS::ENG_NUM_PIECES];

  ArrayPtr
  images() {
    return skeleton_buffer.array_data();
  }

private:
  StaticBuffer<GRAL::Image, ATLAS::ENG_NUM_PIECES> skeleton_buffer;
};

}

#endif
