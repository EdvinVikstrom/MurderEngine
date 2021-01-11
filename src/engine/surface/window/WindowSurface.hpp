#ifndef ME_VULKAN_WINDOW_SURFACE_HPP
  #define ME_VULKAN_WINDOW_SURFACE_HPP

#include "../Surface.hpp"

#include "../../Logger.hpp"

#include <GLFW/glfw3.h>

namespace me {

  class WindowSurface : public Surface {

  private:

    Logger logger;

    Callbacks callbacks;
    GLFWwindow* glfw_window;
    Config config;

  public:

    WindowSurface(Callbacks &callbacks);

    int get_properties(const SurfaceProperty property, uint32_t &count, void* data) const override;
    int get_size(uint32_t &width, uint32_t &height) const override;

    const char** vk_get_required_surface_extensions(uint32_t &count) const override;
    int vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

  protected:

    int initialize(const ModuleInfo) override;
    int terminate(const ModuleInfo) override;
    int tick(const ModuleInfo) override;

    static void glfw_error_callback(int code, const char* description);

  };

}

#endif
