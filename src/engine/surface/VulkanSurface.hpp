#ifndef ME_VULKAN_SURFACE_HPP
  #define ME_VULKAN_SURFACE_HPP

#include "Surface.hpp"

#include <vulkan/vulkan.h>

namespace me {

  class VulkanSurface : public Surface {

  public:

    VulkanSurface(const MurderEngine* engine, const string &name, Config config)
      : Surface(engine, name, config)
    {
    }

    virtual const char** get_extensions(uint32_t &count) const = 0;

    virtual int create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;

  };

}

#endif
