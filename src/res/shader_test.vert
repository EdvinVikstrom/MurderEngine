#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 frag_color;

vec3 vertices[3] = {
  {0.0F, -0.5F, 0.0F},
  {0.5F, 0.5F, 0.0F},
  {-0.5F, 0.5F, 0.0F}
};

void main() {
  gl_Position = vec4(vertices[gl_VertexIndex], 0.0F);
  frag_color = in_color;
}
