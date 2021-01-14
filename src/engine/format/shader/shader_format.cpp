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
