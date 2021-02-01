#ifndef ME_FORMAT_COLLADA_HPP
  #define ME_FORMAT_COLLADA_HPP

#include "../Format.hpp"

#include <lme/map.hpp>

#include <rapidxml/rapidxml.hpp>

namespace me {

  enum ColladaEffectParamType {
    COLLADA_EFFECT_PARAM_SURFACE_2D,
    COLLADA_EFFECT_PARAM_SAMPLER_2D,
    COLLADA_EFFECT_PARAM_NONE
  };

  enum ColladaEffectTechnique {
    COLLADA_EFFECT_TECHNIQUE_LAMBERT,
    COLLADA_EFFECT_TECHNIQUE_PHONG
  };


  struct ColladaEffect {
    string identifier;
    map<string, pair<ColladaEffectParamType, const char*>> params;
    ColladaEffectTechnique technique;
    MaterialColor emission;
    MaterialColor diffuse;
    MaterialColor ior;
  };

  struct ColladaImage {
    string identifier;
    string name;
    Image* image;
  };

  struct ColladaMaterial {
    string identifier;
    string name;
    ColladaEffect* effect;
  };

  struct ColladaGeometry {
    string identifier;
    string name;
    map<string, float*> arrays;
  };

  struct ColladaContext {
    const File &file;
    allocator &alloc;
    map<string, ColladaImage> images;
    map<string, ColladaEffect> effects;
    map<string, ColladaMaterial> materials;
    map<string, ColladaGeometry> geometries;
  };

  class ColladaFormat : public Format {

  public:

    ColladaFormat();

    int read(const me::File &file, size_t len, const char* data, allocator &alloc, Scene &scene);
    int parse_geometry(ColladaContext &context, rapidxml::xml_node<>* geometry_node, ColladaGeometry &geometry);
    int parse_material(ColladaContext &context, rapidxml::xml_node<>* material_node, ColladaMaterial &material);
    int parse_image(ColladaContext &context, rapidxml::xml_node<>* image_node, ColladaImage &image);
    int parse_effect(ColladaContext &context, rapidxml::xml_node<>* effect_node, ColladaEffect &effect);
    int parse_color(ColladaContext &context, rapidxml::xml_base<>* color_node, MaterialColor &color);

    bool recognize(const me::File &file) override;

  };

}

#endif
