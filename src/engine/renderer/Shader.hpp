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

    typedef size_t Link;

  protected:

    const ShaderType type;
    const size_t length;
    const char* data;

    mutable Link link;

  public:

    Shader(const string &source, const ShaderType type, const size_t length, const char* data, Link link = -1)
      : type(type), length(length), data(data)
    {
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
