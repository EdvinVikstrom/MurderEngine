#ifndef ME_SCENE_ITEM_HPP
  #define ME_SCENE_ITEM_HPP

#include "Mesh.hpp"

namespace me {

  class Item {

  private:

    math::vec3f position, rotation, scale;

    MeshReference* mesh;

  public:

    explicit Item(math::vec3f position, math::vec3f rotation, math::vec3f scale, MeshReference* mesh)
    {
      this->position = position;
      this->rotation = rotation;
      this->scale = scale;
      this->mesh = mesh;
    }

    math::vec3f& get_position()
    { return position; }

    math::vec3f& get_rotation()
    { return rotation; }

    math::vec3f& get_scale()
    { return scale; }

    MeshReference* get_mesh()
    { return mesh; }

  };

}

#endif
