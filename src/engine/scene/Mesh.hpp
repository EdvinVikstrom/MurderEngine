#ifndef ME_MESH_HPP
  #define ME_MESH_HPP

#include <lme/math/vector.hpp>
#include <lme/vector.hpp>
#include <lme/string.hpp>

#ifdef ME_USE_VULKAN
  #include <vulkan/vulkan.h>
#endif

namespace me {

  struct Vertex {
    math::vec3f position;
    math::vec3f normal;
    math::vec2f texture;
    math::vec4f color;
  };

  class MeshReference {
    
  protected:

    const string source;
    bool complete;

  public:

    explicit MeshReference(const string &source, bool complete = false)
      : source(source)
    {
      this->complete = complete;
    }

    const string& get_source() const
    { return source; }

    const bool is_complete() const
    { return complete; }

  };

  class Mesh : public MeshReference {

  protected:

#ifdef ME_USE_VULKAN
    typedef VkBuffer VertexBuffer;
#endif

  public:

    const vector<Vertex> vertices;

    VertexBuffer vertex_buffer;
    
    Mesh(const string &source, const vector<Vertex> &vertices)
      : MeshReference(source, true), vertices(vertices)
    {
    }

  };

}

#endif
