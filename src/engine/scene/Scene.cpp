#include "Scene.hpp"

#include "lme/math/math.hpp"
#include "lme/math/matrix.hpp"
#include "lme/math/matrix/matrix4.hpp"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

me::Scene::Scene(const SceneInfo &scene_info, MemoryAlloc &alloc, Camera &camera)
  : scene_info(scene_info), alloc(alloc), camera(camera)
{
}

const me::SceneUniform& me::Scene::get_uniform() const
{
  return uniform;
}

const me::Camera& me::Scene::get_camera() const
{
  return camera;
}

int me::Scene::refresh_options(uint16_t options)
{
  if (options & SCENE_OPTION_FOV_TYPE_BIT || options & SCENE_OPTION_ZNEAR_TYPE_BIT || options & SCENE_OPTION_ZFAR_TYPE_BIT)
  {
    //uint32_t width, height;
    //renderer->get_framebuffer_size(width, height);
    //refresh_projection_matrix(width, height);
  }
  return 0;
}

int me::Scene::initialize()
{
  math::identify_mat4(uniform.model);
  math::identify_mat4(uniform.view);
  math::identify_mat4(uniform.projection);

  math::look_at_mat4(uniform.view, math::vec3f(2.0f, 2.0f, 2.0f), math::vec3f(0.0f, 0.0f, 0.0f), math::vec3f(0.0f, 0.0f, 1.0f));
  camera.refresh_matrices();
  uniform.projection = camera.get_projection_matrix();

  glm::mat4 model = glm::rotate(glm::mat4(1.0f), 1.0F * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  uniform.model[0] = model[0][0];
  uniform.model[1] = model[0][1];
  uniform.model[2] = model[0][2];
  uniform.model[3] = model[0][3];
  uniform.model[4] = model[1][0];
  uniform.model[5] = model[1][1];
  uniform.model[6] = model[1][2];
  uniform.model[7] = model[1][3];
  uniform.model[8] = model[2][0];
  uniform.model[9] = model[2][1];
  uniform.model[10] = model[2][2];
  uniform.model[11] = model[2][3];
  uniform.model[12] = model[3][0];
  uniform.model[13] = model[3][1];
  uniform.model[14] = model[3][2];
  uniform.model[15] = model[3][3];

  uniform.view[0] = view[0][0];
  uniform.view[1] = view[0][1];
  uniform.view[2] = view[0][2];
  uniform.view[3] = view[0][3];
  uniform.view[4] = view[1][0];
  uniform.view[5] = view[1][1];
  uniform.view[6] = view[1][2];
  uniform.view[7] = view[1][3];
  uniform.view[8] = view[2][0];
  uniform.view[9] = view[2][1];
  uniform.view[10] = view[2][2];
  uniform.view[11] = view[2][3];
  uniform.view[12] = view[3][0];
  uniform.view[13] = view[3][1];
  uniform.view[14] = view[3][2];
  uniform.view[15] = view[3][3];
  return 0;
}

int me::Scene::tick()
{
  return 0;
}

int me::Scene::terminate()
{
  return 0;
}


int me::Scene::refresh_projection_matrix(uint32_t width, uint32_t height)
{
  camera.refresh_matrices();
  //math::perspective_mat4(uniform.projection, math::radians(options.fov), (float) width / (float) height, options.z_near, options.z_far);
  //uniform.projection[5] *= -1;
  return 0;
}


int me::Scene::framebuffer_resize_callback(uint32_t width, uint32_t height, void* ptr)
{
  Scene* scene = reinterpret_cast<Scene*>(ptr);
  scene->refresh_projection_matrix(width, height);
  return 0;
}

int me::Scene::input_key_callback(InputEventAction action, InputEventKey key, void* ptr)
{
  return 0;
}

int me::Scene::input_cursor_position_callback(double x_pos, double y_pos, void* ptr)
{
  return 0;
}
