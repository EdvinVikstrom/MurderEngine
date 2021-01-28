#ifndef ME_MESH_HPP
  #define ME_MESH_HPP

#include <lme/math/vector.hpp>
#include <lme/vector.hpp>
#include <lme/string.hpp>

#ifdef ME_USE_VULKAN
  #include <vulkan/vulkan.h>
#endif

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
  typedef uint32_t Index;

  /* mesh class for storing vertices, indices and materials */
  class Mesh {

  protected:

#ifdef ME_USE_VULKAN
    typedef VkDeviceMemory VertexBufferMemory;
    typedef VkDeviceMemory IndexBufferMemory;
    typedef VkBuffer VertexBuffer;
    typedef VkBuffer IndexBuffer;
#endif

  public:

    const vector<Vertex> vertices;
    const vector<Index> indices;

    VertexBufferMemory vertex_buffer_memory;
    IndexBufferMemory index_buffer_memory;
    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;
    
    Mesh(const vector<Vertex> &vertices, const vector<Index> &indices)
      : vertices(vertices), indices(indices)
    {
    }

  };

  /* mesh reference class for storing a pointer to a mesh and flags */
  class MeshRef {

  public:

    Mesh* mesh;
    uint16_t flags;

    MeshRef(Mesh* mesh, uint16_t flags)
    {
      this->mesh = mesh;
      this->flags = flags;
    }

  };

}

#endif
