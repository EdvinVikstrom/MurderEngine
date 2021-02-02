#ifndef ME_SHADER_HPP
  #define ME_SHADER_HPP

#include <lme/string.hpp>

namespace me {

  enum ShaderType {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_GEOMETRY
  };


  struct ShaderData {
    size_t size;
    const char* code;
  };

  struct ShaderConfig {
    const char* entry_point;
  };

  class Shader {

  public:

    const ShaderType type;
    const ShaderData data;
    const ShaderConfig config;

    Shader(ShaderType type, const ShaderData &data, const ShaderConfig &config)
      : type(type), data(data), config(config)
    {
    }

  };

}

#endif
