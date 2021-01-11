#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

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

    struct Callbacks {
      int (*init_surface) (Config&);
    };

  protected:

    Callbacks callbacks;
    Config config;

  public:

    explicit Surface(const string &name, Callbacks &callbacks)
      : Module(MODULE_SURFACE_TYPE, name)
    {
      this->callbacks.init_surface = callbacks.init_surface;
    }

    virtual int get_properties(const SurfaceProperty property, uint32_t &count, void* data) const = 0;
    virtual int get_size(uint32_t &width, uint32_t &height) const = 0;

#ifdef ME_USE_VULKAN
    virtual const char** vk_get_required_surface_extensions(uint32_t &count) const = 0;
    virtual int vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;
#endif

  };

}

#endif
