#include "WindowSurface.hpp"
#include "GLFW/glfw3.h"
#include "WindowSurface.hpp"

#include "../../util/vk_utils.hpp"

#include <lme/time.hpp>

me::WindowSurface::WindowSurface(Callbacks &callbacks)
  : Surface("glfw", callbacks), logger("Window")
{
}

int me::WindowSurface::initialize(const ModuleInfo module_info)
{
  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit())
    throw exception("failed to initialize GLFW");

  logger.info("using GLFW version [%s]", glfwGetVersionString());

  Surface::callbacks.init_surface(Surface::config);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfw_window = glfwCreateWindow(Surface::config.width, Surface::config.height, Surface::config.title, nullptr, nullptr);
  return 0;
}

int me::WindowSurface::terminate(const ModuleInfo module_info)
{
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  return 0;
}

int me::WindowSurface::tick(const ModuleInfo module_info)
{
  if (glfwWindowShouldClose(glfw_window))
  {
    module_info.semaphore->flags |= MODULE_SEMAPHORE_TERMINATE_FLAG;
    return 1;
  }

  glfwPollEvents();
  return 0;
}

int me::WindowSurface::get_properties(const SurfaceProperty property, uint32_t &count, void* data) const
{
  return 0;
}

int me::WindowSurface::get_size(uint32_t &width, uint32_t &height) const
{
  glfwGetWindowSize(glfw_window,
      reinterpret_cast<int*>(&width),
      reinterpret_cast<int*>(&height));
  return 0;
}

const char** me::WindowSurface::vk_get_required_surface_extensions(uint32_t &count) const
{
  return glfwGetRequiredInstanceExtensions(&count);
}

int me::WindowSurface::vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
  VkResult result = glfwCreateWindowSurface(instance, glfw_window, allocator, surface);
  if (result != VK_SUCCESS)
    throw exception("failed to create vulkan surface [%s]", vk_utils_result_string(result));
  return 0;
}


void me::WindowSurface::glfw_error_callback(int code, const char* description)
{
  const char* code_str = "";
  switch (code)
  {
    case GLFW_NO_ERROR: code_str = "NO_ERROR"; break;
    case GLFW_NOT_INITIALIZED: code_str = "NOT_INITIALIZED"; break;
    case GLFW_NO_CURRENT_CONTEXT: code_str = "NO_CURRENT_CONTEXT"; break;
    case GLFW_INVALID_ENUM: code_str = "INVALID_ENUM"; break;
    case GLFW_INVALID_VALUE: code_str = "INVALID_VALUE"; break;
    case GLFW_OUT_OF_MEMORY: code_str = "OUT_OF_MEMORY"; break;
    case GLFW_API_UNAVAILABLE: code_str = "API_UNAVAILABLE"; break;
    case GLFW_VERSION_UNAVAILABLE: code_str = "VERSION_UNAVAILABLE"; break;
    case GLFW_PLATFORM_ERROR: code_str = "PLATFORM_ERROR"; break;
    case GLFW_FORMAT_UNAVAILABLE: code_str = "FORMAT_UNAVAIABLE"; break;
    case GLFW_NO_WINDOW_CONTEXT: code_str = "NO_WINDOW_CONTEXT"; break;
  }

  printf("glfw error [%s]: %s", code_str, description);
}
