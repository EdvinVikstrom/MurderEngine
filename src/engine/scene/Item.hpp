#ifndef ME_SCENE_ITEM_HPP
  #define ME_SCENE_ITEM_HPP

#include "Mesh.hpp"

namespace me {

  class Item {

  private:

    vec3f position, rotation, scale;

    MeshReference* mesh;

  public:

    explicit Item(vec3f position, vec3f rotation, vec3f scale, MeshReference* mesh)
    {
      this->position = position;
      this->rotation = rotation;
      this->scale = scale;
      this->mesh = mesh;
    }

    vec3f& get_position()
    { return position; }

    vec3f& get_rotation()
    { return rotation; }

    vec3f& get_scale()
    { return scale; }

    MeshReference* get_mesh()
    { return mesh; }

  };

}

#endif
