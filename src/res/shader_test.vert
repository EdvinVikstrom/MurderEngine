#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 frag_color;

vec2 positions[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[4] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0)
);



void main()
{
  //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0F);
  //gl_Position = vec4(in_position, 1.0F);
  //frag_color = in_color;

  gl_Position = vec4(positions[gl_VertexIndex], 0.0F, 1.0F);

  frag_color = vec4(colors[gl_VertexIndex], 1.0F);
  if (in_color.x == 1.0)
  {
    if (gl_VertexIndex == 2)
      frag_color = vec4(0.0, 1.0, 0.0, 1.0F);
  }
}
