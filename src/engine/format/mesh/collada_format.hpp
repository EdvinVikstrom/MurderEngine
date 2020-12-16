#ifndef LIBME_FORMAT_COLLADA_HPP
  #define LIBME_FORMAT_COLLADA_HPP

#include <lme/buffer.hpp>

namespace me::format {

  struct Mesh_Result {
  };

  int mesh_collada_read(const Buffer &buffer, Mesh_Result &result);

}

#endif
