#include "VKWindowSurface.hpp"
#include "VKWindowSurface.hpp"

#include "../../util/vk_utils.hpp"

#include <lme/time.hpp>

me::VKWindowSurface::VKWindowSurface(const MurderEngine* engine, Config config)
  : VulkanSurface(engine, "glfw", config)
{
}

int me::VKWindowSurface::create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
  VkResult result = glfwCreateWindowSurface(instance, glfw_window, allocator, surface);
  if (result != VK_SUCCESS)
    throw exception("failed to create window surface [%s]", vk_utils_result_string(result));

  return 0;
}

int me::VKWindowSurface::initialize()
{
  /* make a logger for GLFW */
  logger = engine->get_logger().child("Vulkan-Window");

  if (!glfwInit())
    throw exception("failed to initialize GLFW");

  uint32_t width = 1550, height = 770;
  const char* title = engine->get_app_config().name;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  return 0;
}

int me::VKWindowSurface::terminate()
{
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  return 0;
}

int me::VKWindowSurface::tick(const Context context)
{
  if (glfwWindowShouldClose(glfw_window))
    return 1;

  glfwPollEvents();
  sleep = 1000L / config.fps;
  return 0;
}

const char** me::VKWindowSurface::get_extensions(uint32_t &count) const
{
  const char** extensions = glfwGetRequiredInstanceExtensions(&count);

  if (extensions == nullptr)
    throw exception("failed to get GLFW required instance extensions");

  return extensions;
}

int me::VKWindowSurface::get_size(uint32_t &width, uint32_t &height) const
{
  glfwGetWindowSize(glfw_window, (int*) &width, (int*) &height);
  return 0;
}
