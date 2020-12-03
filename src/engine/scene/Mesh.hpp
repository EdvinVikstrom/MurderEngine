#ifndef ME_MESH_HPP
  #define ME_MESH_HPP

#include "../math/Vector.hpp"

#include <vector>

namespace me {

  struct Vertex {
  };

  struct Mesh {

    vec3f pos, scale, rot;

    const Vertex* vertices;
    
    Mesh(vec3f &pos, vec3f &scale, vec3f &rot, const Vertex* vertices)
      : vertices(vertices)
    {
      this->pos = pos;
      this->scale = scale;
      this->rot = rot;
    }

  };

}

#endif
