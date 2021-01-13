#include "AndroidSurface.hpp"
#include "android/native_window.h"
#include <vulkan/vulkan_android.h>

me::AndroidSurface::AndroidSurface(Callbacks &callbacks)
  : Surface("", callbacks), logger("android")
{
}

int me::AndroidSurface::initialize(const ModuleInfo)
{
  return 0;
}

int me::AndroidSurface::terminate(const ModuleInfo)
{
  return 0;
}

int me::AndroidSurface::tick(const ModuleInfo)
{
  return 0;
}


int me::AndroidSurface::get_properties(const SurfaceProperty property, uint32_t &count, void* data) const
{
  return 0;
}

int me::AndroidSurface::get_size(uint32_t &width, uint32_t &height) const
{
  width = ANativeWindow_getWidth(android_window);
  height = ANativeWindow_getHeight(android_window);
  return 0;
}

const char** me::AndroidSurface::vk_get_required_surface_extensions(uint32_t &count) const
{
  static const char* required_extensions[1] { "VK_KHR_android_surface" };
  return required_extensions;
}

int me::AndroidSurface::vk_create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const
{
  VkAndroidSurfaceCreateInfoKHR android_surface_create_info = { };
  android_surface_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  android_surface_create_info.pNext = nullptr;
  android_surface_create_info.flags = 0;
  android_surface_create_info.window = android_window;

  VkResult result = vkCreateAndroidSurfaceKHR(instance, &android_surface_create_info, allocator, surface);
  return 0;
}
