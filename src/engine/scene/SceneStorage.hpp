#ifndef ME_SCENE_STORAGE_HPP
  #define ME_SCENE_STORAGE_HPP

#include "Mesh.hpp"

#include <lme/vector.hpp>

namespace me {

  struct SceneStorage {
    vector<MeshRef> meshes;
  };

}

#endif
