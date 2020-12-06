#ifndef ME_MESH_HPP
  #define ME_MESH_HPP

#include "../math/Vector.hpp"

#include <vector>
#include <string>

namespace me {

  struct Vertex {
    vec3f position;
    vec3f normal;
    vec2f texture;
    vec4f color;
  };

  class MeshReference {
    
  private:

    const std::string source;
    bool complete;

  public:

    explicit MeshReference(const std::string &source, bool complete = false)
      : source(source)
    {
      this->complete = complete;
    }

    const std::string& get_source() const
    { return source; }

    const bool is_complete() const
    { return complete; }

  };

  class Mesh : public MeshReference {

  private:

    const std::vector<Vertex> vertices;

  public:
    
    Mesh(const std::string &source, const std::vector<Vertex> &vertices)
      : MeshReference(source, true), vertices(vertices)
    {
    }

  };

}

#endif
