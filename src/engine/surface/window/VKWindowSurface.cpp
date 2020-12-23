#include "VKWindowSurface.hpp"
#include "GLFW/glfw3.h"
#include "VKWindowSurface.hpp"

#include "../../util/vk_utils.hpp"

#include <lme/time.hpp>

me::VKWindowSurface::VKWindowSurface()
  : VulkanSurface("glfw"), logger("VK-Window")
{
}

int me::VKWindowSurface::create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
  VkResult result = glfwCreateWindowSurface(instance, glfw_window, allocator, surface);
  if (result != VK_SUCCESS)
    throw exception("failed to create window surface [%s]", vk_utils_result_string(result));

  return 0;
}

int me::VKWindowSurface::initialize(const ModuleInfo module_info)
{
  if (!glfwInit())
    throw exception("failed to initialize GLFW");

  uint32_t width = 1550, height = 770;
  const char* title = "title";

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  glfwSwapInterval(1);
  return 0;
}

int me::VKWindowSurface::terminate(const ModuleInfo module_info)
{
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  return 0;
}

int me::VKWindowSurface::tick(const ModuleInfo module_info)
{
  if (glfwWindowShouldClose(glfw_window))
  {
    module_info.semaphore->flags |= MODULE_SEMAPHORE_TERMINATE_FLAG;
    return 1;
  }

  glfwPollEvents();
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

size_t me::VKWindowSurface::get_current_frame_index() const
{
  return frame_index;
}
