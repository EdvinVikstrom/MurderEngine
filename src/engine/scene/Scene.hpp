#ifndef ME_SCENE_HPP
  #define ME_SCENE_HPP

#include "SceneInfo.hpp"
#include "SceneStorage.hpp"
#include "Camera.hpp"

#include "../memory/MemoryPool.hpp"
#include "../renderer/Renderer.hpp"
#include "../event/InputEvent.hpp"

namespace me {

  enum SceneOptionTypeBit {
    SCENE_OPTION_FOV_TYPE_BIT = 1,
    SCENE_OPTION_ZNEAR_TYPE_BIT = 1 << 1,
    SCENE_OPTION_ZFAR_TYPE_BIT = 1 << 2
  };


  struct SceneUniform {
    math::mat4f model;
    math::mat4f view;
    math::mat4f projection;
  };

  class Scene {

  protected:

    const SceneInfo scene_info;

    MemoryAlloc alloc;

    SceneStorage storage;
    SceneUniform uniform;
    Camera camera;

  public:

    Scene(const SceneInfo &scene_info, MemoryAlloc &alloc, Camera &camera);

    const SceneUniform& get_uniform() const;
    const Camera& get_camera() const;

    int refresh_options(uint16_t options);

    int initialize();
    int tick();
    int terminate();

  protected:

    int refresh_projection_matrix(uint32_t width, uint32_t height);

    static int framebuffer_resize_callback(uint32_t width, uint32_t height, void* ptr);

    static int input_key_callback(InputEventAction action, InputEventKey key, void* ptr);
    static int input_cursor_position_callback(double x_pos, double y_pos, void* ptr);

  };

}

#endif
