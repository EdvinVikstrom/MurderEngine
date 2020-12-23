#ifndef ME_SHADER_HPP
  #define ME_SHADER_HPP

#include <lme/string.hpp>

namespace me {

  enum ShaderType : uint8_t {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_GEOMETRY
  };


  class Shader {

  public:

    struct Config {
      const char* entry_point;
    };

  public:

    typedef size_t Link;

  protected:

    const ShaderType type;
    const size_t length;
    const char* data;

    mutable Config config;
    mutable Link link;

  public:

    Shader(const string &source, const ShaderType type, const size_t length, const char* data, Config config, Link link = -1)
      : type(type), length(length), data(data)
    {
      this->config = config;
      this->link = link;
    }

    const ShaderType get_type() const
    {
      return type;
    }

    const size_t get_length() const
    {
      return length;
    }

    const char* get_data() const
    {
      return data;
    }

    Config& get_config() const
    {
      return config;
    }

    Link get_link() const
    {
      return link;
    }

    void set_link(Link link) const
    {
      this->link = link;
    }

  };

}

#endif
