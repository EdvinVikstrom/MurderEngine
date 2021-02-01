#ifndef ME_SCENE_HPP
  #define ME_SCENE_HPP

#include "Mesh.hpp"
#include "Camera.hpp"

namespace me {

  class Scene {

  public:

    vector<Material*> materials;
    vector<MeshItem*> meshes;
    vector<Camera*> cameras;
  
  };

}

#endif
