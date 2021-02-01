#ifndef ME_MATERIAL_HPP
  #define ME_MATERIAL_HPP

#include "../resources/Image.hpp"

#include <lme/string.hpp>
#include <lme/math/vector.hpp>

namespace me {

  enum MaterialColorType {
    MATERIAL_COLOR_IMAGE,
    MATERIAL_COLOR_RGBA,
    MATERIAL_COLOR_SINGLE
  };


  class MaterialColor {

  public:

    MaterialColorType type;
    union {
      Image* image;
      math::vec4f rgba;
      float single;
    };

  };

  class Material {

  public:

    string identifier;
    MaterialColor diffuse;
    MaterialColor emission;

  };

}

#endif
