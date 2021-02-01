#ifndef ME_FORMAT_MES_HPP
  #define ME_FORMAT_MES_HPP

#include "../Format.hpp"

namespace me {

  class MESFormat : public Format {

  public:

    MESFormat();

    int read(const me::File &file, size_t len, const char* data, allocator &alloc, Scene &scene);

    bool recognize(const me::File &file) override;

  };

}

#endif
