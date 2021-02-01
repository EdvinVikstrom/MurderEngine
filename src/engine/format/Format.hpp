#ifndef ME_FORMAT_HPP
  #define ME_FORMAT_HPP

#include "../resources/Image.hpp"
#include "../scene/Scene.hpp"

#include <lme/file.hpp>

namespace me {

  enum FormatType {
    FORMAT_TYPE_IMAGE,
    FORMAT_TYPE_MESH,
    FORMAT_TYPE_SCENE
  };


  class Format {

  protected:

    FormatType type;
    const string identifier;

  public:

    Format(FormatType type, const string &identifier)
      : type(type), identifier(identifier)
    {
    }

    virtual bool recognize(const me::File &file) = 0;

    static Image* read_image(const me::File &file, allocator &alloc);

  };

}

#endif
