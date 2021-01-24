#ifndef ME_SCENE_INFO_HPP
  #define ME_SCENE_INFO_HPP

#include "../memory/MemoryAlloc.hpp"
#include "../renderer/Renderer.hpp"

namespace me {

  enum SceneType {
    SCENE_2D_VIEWPORT_TYPE,
    SCENE_3D_VIEWPORT_TYPE,
    SCENE_UI_TYPE
  };


  struct SceneInfo {
    SceneType type;
  };

}

#endif
