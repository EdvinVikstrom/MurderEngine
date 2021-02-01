#include "collada.hpp"
#include "rapidxml/rapidxml.hpp"

me::ColladaFormat::ColladaFormat()
  : Format(FORMAT_TYPE_SCENE, "khronos:collada")
{
}

int me::ColladaFormat::read(const me::File &file, size_t len, const char* data, allocator &alloc, Scene &scene)
{
  ColladaContext context = {file, alloc};

  char* iter = const_cast<char*>(data);

  rapidxml::xml_document<> document;
  document.parse<0>(iter);

  rapidxml::xml_node<>* collada_node = document.first_node("COLLADA");

  rapidxml::xml_node<>* image_nodes = collada_node->first_node("library_images");
  if (image_nodes)
  {
    rapidxml::xml_node<>* image_node = image_nodes->first_node();
    while (image_node)
    {
      ColladaImage image = {};
      parse_image(context, image_node, image);
      context.images[image.identifier] = image;
      image_node = image_node->next_sibling();
    }
  }
 
  rapidxml::xml_node<>* effect_nodes = collada_node->first_node("library_effects");
  if (effect_nodes)
  {
    rapidxml::xml_node<>* effect_node = effect_nodes->first_node();
    while (effect_node)
    {
      ColladaEffect effect = {};
      parse_effect(context, effect_node, effect);
      context.effects[effect.identifier] = effect;
      effect_node = effect_node->next_sibling();
    }
  } 

  rapidxml::xml_node<>* material_nodes = collada_node->first_node("library_materials");
  if (material_nodes)
  {
    rapidxml::xml_node<>* material_node = material_nodes->first_node();
    while (material_node)
    {
      ColladaMaterial material = {};
      parse_material(context, material_node, material);
      context.materials[material.identifier] = material;
      material_node = material_node->next_sibling();
    }
  }

  rapidxml::xml_node<>* geometry_nodes = collada_node->first_node("library_geometries");
  if (geometry_nodes)
  {
    rapidxml::xml_node<>* geometry_node = geometry_nodes->first_node();
    while (geometry_node)
    {
      ColladaGeometry geometry = {};
      parse_geometry(context, geometry_node, geometry);
      context.geometries[geometry.identifier] = geometry;
      geometry_node = geometry_node->next_sibling();
    }
  }
  return 0;
}

int me::ColladaFormat::parse_geometry(ColladaContext &context, rapidxml::xml_node<>* geometry_node, ColladaGeometry &geometry)
{
  rapidxml::xml_attribute<>* id_attrib = geometry_node->first_attribute("id");
  rapidxml::xml_attribute<>* name_attrib = geometry_node->first_attribute("name");
  geometry.identifier = id_attrib->value();
  geometry.name = name_attrib->value();

  rapidxml::xml_node<>* mesh_node = geometry_node->first_node("mesh");
  if (mesh_node)
  {
    rapidxml::xml_node<>* source_node = mesh_node->first_node("source");
    while (source_node)
    {
      rapidxml::xml_node<>* node = source_node->first_node();

      if (strcmp("float_array", node->name()) == 0)
      {
	string identifier = node->first_attribute("id")->value();
	string value = node->value();

	size_t float_count;
	value.split(' ', float_count, nullptr);
	string_view floats[float_count];
	value.split(' ', float_count, floats);
      }

      source_node = source_node->next_sibling();
    }
  }
  return 0;
}

int me::ColladaFormat::parse_material(ColladaContext &context, rapidxml::xml_node<>* material_node, ColladaMaterial &material)
{
  rapidxml::xml_attribute<>* id_attrib = material_node->first_attribute("id");
  rapidxml::xml_attribute<>* name_attrib = material_node->first_attribute("name");
  material.identifier = id_attrib->value();
  material.name = name_attrib->value();

  rapidxml::xml_node<>* node = material_node->first_node();
  if (strcmp("instance_effect", node->name()) == 0)
  {
    string url = node->first_attribute("url")->value();
    material.effect = &context.effects[url.substr(1)];
  }
  return 0;
}

int me::ColladaFormat::parse_image(ColladaContext &context, rapidxml::xml_node<>* image_node, ColladaImage &image)
{
  rapidxml::xml_attribute<>* id_attrib = image_node->first_attribute("id");
  rapidxml::xml_attribute<>* name_attrib = image_node->first_attribute("name");
  image.identifier = id_attrib->value();
  image.name = name_attrib->value();

  rapidxml::xml_node<>* node = image_node->first_node();
  if (strcmp("init_from", node->name()) == 0)
  {
    image.image = Format::read_image(File(context.file.get_directory_path(), node->value()), context.alloc);
  }
  return 0;
}

int me::ColladaFormat::parse_effect(ColladaContext &context, rapidxml::xml_node<>* effect_node, ColladaEffect &effect)
{
  rapidxml::xml_attribute<>* id_attrib = effect_node->first_attribute("id");
  effect.identifier = id_attrib->value();

  rapidxml::xml_node<>* profile_node = effect_node->first_node("profile_COMMON");

  rapidxml::xml_node<>* node = profile_node->first_node();
  while (node)
  {
    if (strcmp("newparam", node->name()) == 0)
    {
      const char* param_name = node->first_attribute("sid")->value();
      ColladaEffectParamType param_type;
      const char* param_value;

      node = node->first_node();
      if (strcmp("surface", node->name()) == 0)
	param_type = COLLADA_EFFECT_PARAM_SURFACE_2D;
      else if (strcmp("sampler2D", node->name()) == 0)
	param_type = COLLADA_EFFECT_PARAM_SAMPLER_2D;
      else
	param_type = COLLADA_EFFECT_PARAM_NONE;

      node = node->first_node();
      if (strcmp("init_from", node->name()) == 0)
	param_value = node->value();
      else if (strcmp("source", node->name()) == 0)
	param_value = effect.params[string(node->value())].second;
      else
	param_value = nullptr;

      effect.params[string(param_name)] = {param_type, param_value};
    }else if (strcmp("technique", node->name()) == 0)
    {
      node = node->first_node();
      if (strcmp("lambert", node->name()) == 0)
	effect.technique = COLLADA_EFFECT_TECHNIQUE_LAMBERT;
      if (strcmp("phong", node->name()) == 0)
	effect.technique = COLLADA_EFFECT_TECHNIQUE_PHONG;

      rapidxml::xml_node<>* technique_node = node->first_node();
      while ((node = technique_node->next_sibling()))
      {
	rapidxml::xml_node<>* color_node = node->first_node();
	if (strcmp("diffuse", node->name()) == 0)
	  parse_color(context, color_node, effect.diffuse);
	else if (strcmp("emission", node->name()) == 0)
	  parse_color(context, color_node, effect.emission);
	else if (strcmp("index_of_refraction", node->name()) == 0)
	  parse_color(context, color_node, effect.ior);
      }
    }

    node = node->next_sibling();
  }
  return 0;
}

int me::ColladaFormat::parse_color(ColladaContext &context, rapidxml::xml_base<>* color_node, MaterialColor &color)
{
  if (strcmp("texture", color_node->name()) == 0)
  {
    color.type = MATERIAL_COLOR_IMAGE;
    color.image = context.images[string(color_node->value())].image;
  }else if (strcmp("color", color_node->name()) == 0)
  {
    string float_str = color_node->value();
    size_t float_count;
    float_str.split(' ', float_count, nullptr);
    string_view floats[float_count];
    float_str.split(' ', float_count, floats);

    char temp[128];
    float r = float_count >= 1 ? strfloat(floats[0].size(), floats[0].c_str(temp)) : 0.0F;
    float g = float_count >= 2 ? strfloat(floats[1].size(), floats[0].c_str(temp)) : 0.0F;
    float b = float_count >= 3 ? strfloat(floats[2].size(), floats[0].c_str(temp)) : 0.0F;
    float a = float_count >= 4 ? strfloat(floats[3].size(), floats[0].c_str(temp)) : 1.0F;
    color.type = MATERIAL_COLOR_RGBA;
    color.rgba = {r, g, b, a};
  }else if (strcmp("float", color_node->name()) == 0)
  {
    color.type = MATERIAL_COLOR_SINGLE;
    color.single = strfloat(color_node->value_size(), color_node->value());
  }
  return 0;
}

bool me::ColladaFormat::recognize(const me::File &file)
{
  if (!file.has_suffix())
    return false;
  const string suffix = file.get_suffix();
  return suffix == "dae" || suffix == "collada";
}
