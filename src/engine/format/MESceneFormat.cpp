#include "MESceneFormat.hpp"
#include <lme/bytebuff_impl.hpp>

int me::format::write_scene(Bytebuff &buffer, const MEScene &scene)
{
  uint8_t header_bools = 0x00;
  if (scene.header.mesh_data)
    header_bools |= 1;
  else if (scene.header.image_data)
    header_bools |= (1 << 1);
  else if (scene.header.logic_data)
    header_bools |= (1 << 2);

  buffer.push_uint8(header_bools);

  return 0;
}
