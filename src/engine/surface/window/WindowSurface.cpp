#include "WindowSurface.hpp"

#include "../../renderer/vulkan/Util.hpp"

#include "GLFW/glfw3.h"

#include <lme/time.hpp>

me::WindowSurface::WindowSurface(UserCallbacks &user_callbacks)
  : SurfaceModule("glfw", user_callbacks), logger("Window")
{
}

int me::WindowSurface::initialize(const ModuleInfo module_info)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    throw exception("failed to initialize GLFW");

  logger.info("using GLFW version [%s]", glfwGetVersionString());

  if (user_callbacks.init_surface == nullptr)
    throw exception("'Surface::user_callbacks' cannot be nullptr");
  user_callbacks.init_surface(config);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfw_window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
  glfwSetWindowUserPointer(glfw_window, this);
  glfwSetFramebufferSizeCallback(glfw_window, glfw_framebuffer_size_callback);
  glfwSetWindowRefreshCallback(glfw_window, glfw_window_refresh_callback);
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

int me::WindowSurface::get_framebuffer_size(uint32_t &width, uint32_t &height) const
{
  glfwGetFramebufferSize(glfw_window,
      reinterpret_cast<int*>(&width),
      reinterpret_cast<int*>(&height));
  return 0;
}

int me::WindowSurface::notify() const
{
  glfwRequestWindowAttention(glfw_window);
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
    throw exception("failed to create vulkan surface [%s]", util::get_result_string(result));
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

void me::WindowSurface::glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height)
{
  WindowSurface* instance = reinterpret_cast<WindowSurface*>(glfwGetWindowUserPointer(glfw_window));

  if (instance->user_callbacks.resize_surface != nullptr)
    instance->user_callbacks.resize_surface(width, height);
  for (pair<SurfaceModule::Callbacks::resize_surface_fn*, void*> &resize_surface : instance->callbacks.resize_surface)
    resize_surface.first(width, height, resize_surface.second);
}

void me::WindowSurface::glfw_window_refresh_callback(GLFWwindow* glfw_window)
{
  WindowSurface* instance = reinterpret_cast<WindowSurface*>(glfwGetWindowUserPointer(glfw_window));
}


void me::WindowSurface::glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
{
  WindowSurface* instance = reinterpret_cast<WindowSurface*>(glfwGetWindowUserPointer(glfw_window));

  InputEventAction me_action = glfw_translate_action(action);
  InputEventKey me_key = glfw_translate_key(key);

  for (auto callback : instance->input_event_callbacks)
    callback.first->input_event_key(me_action, me_key, callback.second);
}

void me::WindowSurface::glfw_cursor_position_callback(GLFWwindow* glfw_window, double x_pos, double y_pos)
{
  WindowSurface* instance = reinterpret_cast<WindowSurface*>(glfwGetWindowUserPointer(glfw_window));

  for (auto callback : instance->input_event_callbacks)
    callback.first->input_event_cursor_position(x_pos, y_pos, callback.second);
}

me::InputEventKey me::WindowSurface::glfw_translate_key(int key)
{
  if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    return (InputEventKey) key;
  return INPUT_EVENT_UNKNOWN_KEY;
}

me::InputEventAction me::WindowSurface::glfw_translate_action(int action)
{
  if (action == GLFW_PRESS)
    return INPUT_EVENT_PRESS_ACTION;
  else if (action == GLFW_RELEASE)
    return INPUT_EVENT_RELEASE_ACTION;
  return INPUT_EVENT_UNKNOWN_ACTION;
}
