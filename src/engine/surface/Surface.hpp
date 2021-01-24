#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"
#include "../event/InputEvent.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>
#include <lme/pair.hpp>

#ifdef ME_USE_VULKAN
  #include <vulkan/vulkan.h>
#endif

namespace me {

  enum SurfaceProperty {
  };


  class Surface : public Module {

  public:

    struct Monitor {
      void* ptr = nullptr;
    };

    struct Config {
      char const* title = "";
      uint32_t width, height;
      Monitor monitor;
    };

    struct UserCallbacks {
      typedef int (init_surface_fn) (Config&);
      typedef int (resize_surface_fn) (int width, int height);

      init_surface_fn* init_surface;
      resize_surface_fn* resize_surface;
    };

    struct Callbacks {
      typedef int (resize_surface_fn) (uint32_t width, uint32_t height, void* ptr);

      vector<pair<resize_surface_fn*, void*>> resize_surface;
    };

  protected:

    UserCallbacks user_callbacks;
    Callbacks callbacks;
    Config config;
    
    vector<pair<InputEventCallback*, void*>> input_event_callbacks;

  public:

    explicit Surface(const string &name, UserCallbacks &user_callbacks)
      : Module(MODULE_SURFACE_TYPE, name)
    {
      this->user_callbacks = user_callbacks;
    }

    virtual int get_properties(const SurfaceProperty property, uint32_t &count, void* data) const = 0;
    virtual int get_framebuffer_size(uint32_t &width, uint32_t &height) const = 0;

    virtual int notify() const = 0;

#ifdef ME_USE_VULKAN
    virtual const char** vk_get_required_surface_extensions(uint32_t &count) const = 0;
    virtual int vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;
#endif

    int register_resize_surface_callback(Callbacks::resize_surface_fn* resize_surface, void* ptr)
    {
      callbacks.resize_surface.emplace_back(resize_surface, ptr);
      return 0;
    }

    int register_input_event_callback(InputEventCallback* input_event_callback, void* ptr)
    {
      input_event_callbacks.emplace_back(input_event_callback, ptr);
      return 0;
    }

  };

}

#endif
