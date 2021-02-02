#ifndef ME_SHADER_TOOLS_HPP
  #define ME_SHADER_TOOLS_HPP

namespace me::tools {

  enum ShaderCompiler {
    SHADER_COMPILER_KHRONOS_SPIRV
  };
 

  struct CompileShaderInfo {
    ShaderCompiler compiler;
    size_t data_size;
    const char* data;
  };

  int compile_shader(const CompileShaderInfo &compile_shader_info, size_t &output_len, const char* &output);

}

#endif
