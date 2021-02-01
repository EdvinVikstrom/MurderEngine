#include "mes.hpp"

#include <lme/buffer.hpp>

me::MESFormat::MESFormat()
  : Format(FORMAT_TYPE_SCENE, "murder_engine:scene")
{
}

int me::MESFormat::read(const me::File &file, size_t len, const char* data, allocator &alloc, Scene &scene)
{
  ByteBuffer buffer(len, const_cast<char* const>(data));
  return 0;
}

bool me::MESFormat::recognize(const me::File &file)
{
  if (!file.has_suffix())
    return false;
  const string suffix = file.get_suffix();
  return suffix == "mes";
}
