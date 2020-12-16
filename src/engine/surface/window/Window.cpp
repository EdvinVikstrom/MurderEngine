#include "Window.hpp"

#include "../../util/vk_utils.hpp"

me::Window::Window(const MurderEngine* engine)
  : Surface(engine, "glfw")
{
}

int me::Window::create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
  VkResult result = glfwCreateWindowSurface(instance, glfw_window, allocator, surface);
  if (result != VK_SUCCESS)
    throw exception("failed to create window surface [%s]", vk_utils_result_string(result));

  return 0;
}

int me::Window::initialize()
{
  /* make a logger for GLFW */
  logger = engine->get_logger().child("GLFW");

  if (!glfwInit())
    throw exception("failed to initialize GLFW");

  uint32_t width = 1550, height = 770;
  const char* title = engine->get_app_config().name;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  uint32_t instance_count;
  const char** extensions = glfwGetRequiredInstanceExtensions(&instance_count);

  if (extensions == nullptr)
    throw exception("failed to get GLFW required instance extensions");

  this->extensions.reserve(instance_count);
  for (uint32_t i = 0; i < instance_count; i++)
    this->extensions.push_back(extensions[i]);

  return 0;
}

int me::Window::terminate()
{
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  return 0;
}

int me::Window::tick()
{
  if (glfwWindowShouldClose(glfw_window))
    return 1;
  return 0;
}

int me::Window::get_size(uint32_t &width, uint32_t &height) const
{
  glfwGetWindowSize(glfw_window, (int*) &width, (int*) &height);
  return 0;
}
