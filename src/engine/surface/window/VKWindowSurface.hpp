#ifndef ME_VULKAN_WINDOW_SURFACE_HPP
  #define ME_VULKAN_WINDOW_SURFACE_HPP

#include "../VulkanSurface.hpp"

#include "../../Logger.hpp"

#include <GLFW/glfw3.h>

namespace me {

  class VKWindowSurface : public VulkanSurface {

  private:

    Logger logger;

    GLFWwindow* glfw_window;
    size_t frame_index;

  public:

    VKWindowSurface();

    int get_size(uint32_t &width, uint32_t &height) const override;
    size_t get_current_frame_index() const override;

    const char** get_extensions(uint32_t &count) const override;
    int create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

  protected:

    int initialize(const ModuleInfo) override;
    int terminate(const ModuleInfo) override;
    int tick(const ModuleInfo) override;

  };

}

#endif
