#ifndef ME_MESH_HPP
  #define ME_MESH_HPP

#include "Material.hpp"
#include "../renderer/Types.hpp"

#include <lme/math/vector.hpp>
#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  enum MeshFlag {
    MESH_VISIBLE_FLAG = 1
  };


  struct Vertex {
    math::vec3f position;
    math::vec3f normal;
    math::vec2f tex_coord;
    math::vec4f color;
  };

  struct Index {
    uint32_t index;
  };

  /* mesh class for storing vertices, indices and materials */
  class Mesh {

  public:

    string identifier;
    vector<Vertex> vertices;
    vector<Index> indices;

    Buffer vertex_buffer;
    Buffer index_buffer;

  };

  /* mesh reference class for storing a pointer to a mesh and flags */
  class MeshItem {

  public:

    string identifier;
    Mesh* mesh;
    Material* material;
    math::vec3f position, rotation, scale;
    uint16_t flags;

  };

}

#endif
