#ifndef ME_WINDOW_HPP
  #define ME_WINDOW_HPP

#include "../Surface.hpp"

#include <GLFW/glfw3.h>

namespace me {

  class Window : public Surface {

  private:

    Logger* logger;

    GLFWwindow* glfw_window;

  public:

    Window(const MurderEngine* engine);

    int initialize() override;
    int terminate() override;

    int tick() override;

    int get_size(uint32_t &width, uint32_t &height) const override;

    int create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

  };

}

#endif
