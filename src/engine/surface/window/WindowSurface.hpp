#ifndef ME_VULKAN_WINDOW_SURFACE_HPP
  #define ME_VULKAN_WINDOW_SURFACE_HPP

#include "../Surface.hpp"

#include "../../Logger.hpp"

#include <GLFW/glfw3.h>

namespace me {

  class WindowSurface : public SurfaceModule {

  private:

    Logger logger;

    GLFWwindow* glfw_window;

  public:

    WindowSurface(UserCallbacks &user_callbacks);

    int get_properties(const SurfaceProperty property, uint32_t &count, void* data) const override;
    int get_framebuffer_size(uint32_t &width, uint32_t &height) const override;

    int notify() const override;

    const char** vk_get_required_surface_extensions(uint32_t &count) const override;
    int vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

  protected:

    int initialize(const ModuleInfo) override;
    int terminate(const ModuleInfo) override;
    int tick(const ModuleInfo) override;

    static void glfw_error_callback(int code, const char* description);
    static void glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_window_refresh_callback(GLFWwindow* glfw_window);

    static void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    static void glfw_cursor_position_callback(GLFWwindow* glfw_window, double x_pos, double y_pos);

    static InputEventKey glfw_translate_key(int key);
    static InputEventAction glfw_translate_action(int action);

  };

}

#endif
