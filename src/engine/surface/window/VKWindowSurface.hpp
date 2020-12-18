#ifndef ME_VULKAN_WINDOW_SURFACE_HPP
  #define ME_VULKAN_WINDOW_SURFACE_HPP

#include "../VulkanSurface.hpp"

#include <GLFW/glfw3.h>

namespace me {

  class VKWindowSurface : public VulkanSurface {

  private:

    Logger* logger;

    GLFWwindow* glfw_window;

  public:

    VKWindowSurface(const MurderEngine* engine, Config config);

    int get_size(uint32_t &width, uint32_t &height) const override;

    const char** get_extensions(uint32_t &count) const override;
    int create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

  protected:

    int initialize() override;
    int terminate() override;
    int tick(const Context context) override;

  };

}

#endif
