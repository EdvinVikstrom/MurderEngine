#ifndef ME_SCENE_FORMAT_HPP
  #define ME_SCENE_FORMAT_HPP

#include "../scene/Item.hpp"

#include <lme/bytebuff.hpp>

namespace me::format {

  struct MEScene {

    struct Header {
      const uint8_t mesh_data : 1;
      const uint8_t image_data : 1;
      const uint8_t logic_data : 1;
    } header;

    const std::vector<Item> items; // optional

  };

  int write_scene(Bytebuff &buffer, const MEScene &scene);

}

#endif
