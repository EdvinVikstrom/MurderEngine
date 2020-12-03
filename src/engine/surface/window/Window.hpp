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

    int signal() override;

    int initialize() override;
    int terminate() override;

    int tick() override;

  };

}

#endif
