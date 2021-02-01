#ifndef ME_FORMAT_OBJ_HPP
  #define ME_FORMAT_OBJ_HPP

#include "../Format.hpp"
#include "../../scene/Mesh.hpp"

namespace me {

  class OBJFormat : public Format {

  public:

    struct Result {
      vector<MeshItem*> meshes;
    };

  public:

    OBJFormat();

    int read(const me::File &file, size_t len, const char* data, Result &result);

    bool recognize(const me::File &file) override;

  };

}

#endif
