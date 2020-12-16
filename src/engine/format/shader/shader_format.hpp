#ifndef ME_FORMAT_SHADER_HPP
  #define ME_FORMAT_SHADER_HPP

#include "../../renderer/Shader.hpp"

#include <lme/buffer.hpp>

#include <lme/vector.hpp>

namespace me::format {

  struct Shader_Result {
    vector<Shader*> shaders;
  };


  int shader_read(const string &source, const Buffer &buffer, Shader_Result &result);

}

#endif
