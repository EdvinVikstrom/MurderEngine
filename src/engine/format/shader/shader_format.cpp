#include "shader_format.hpp"

#include "../../Logger.hpp"

#include <lme/string.hpp>

/*___[ SHADER FILE ]___
 *|1
 *|2 [vertex]
 *|3 code
 *|4 code
 *|5 [fragment]
 *|6 code
 *|7 code
 *|____________________
 */

static me::ShaderType get_shader_type(const me::string_view &str)
{
  if (str == "vertex")
    return me::SHADER_VERTEX;
  else if (str == "fragment")
    return me::SHADER_FRAGMENT;
  else if (str == "geometry")
    return me::SHADER_GEOMETRY;
  return me::SHADER_VERTEX;
}

int me::format::shader_read(const string &source, const Buffer &buffer, const ShaderType shader_type, Shader_Result &result)
{
  const size_t data_length = buffer.seek_end();
  const char* data = buffer.pull_all();

  Shader::Config shader_config = {
    .entry_point = "main"
  };
  result.shaders.push_back(new Shader(source, shader_type, data_length, data, shader_config));
  return 0;
}
