#include "Vulkan.hpp"
#include "Util.hpp"

int me::vulkan::Vulkan::setup_instance(const EngineInfo &engine_info, Surface* surface)
{
  /* === Appending required extensions to 'required_extensions' === */

  uint32_t surface_extension_count;
  const char** surface_extensions = surface->vk_get_required_surface_extensions(surface_extension_count);

#ifndef NDEBUG
  required_extensions.reserve(surface_extension_count + 2);
  required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  required_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

  required_layers.reserve(1);
  required_layers.push_back("VK_LAYER_KHRONOS_validation");
#else
  required_extensions.reserve(surface_extension_count);
#endif

  for (uint32_t i = 0; i < surface_extension_count; i++)
    required_extensions.push_back(surface_extensions[i]);


  /* getting supported extensions */
  uint32_t extension_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  VkExtensionProperties extensions[extension_count];
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions);

  /* getting supported layers */
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  VkLayerProperties layers[layer_count];
  vkEnumerateInstanceLayerProperties(&layer_count, layers);

  /* checking if required extensions is supported */
  if (!util::has_required_extensions({extension_count, extensions}, required_extensions))
    throw exception("required extensions not supported");

  /* checking if required layers is supported */
  if (!util::has_required_layers({layer_count, layers}, required_layers))
    throw exception("required layers not supported");


  /* === Creating the vulkan instance */
  VkApplicationInfo application_info = { };
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pNext = nullptr;
  application_info.pApplicationName = engine_info.application_info.name;
  application_info.applicationVersion = engine_info.application_info.version;
  application_info.pEngineName = ME_ENGINE_NAME;
  application_info.engineVersion = VK_MAKE_VERSION(ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo instance_create_info = { };
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = 0;
  instance_create_info.pApplicationInfo = &application_info;
  instance_create_info.enabledLayerCount = required_layers.size();
  instance_create_info.ppEnabledLayerNames = required_layers.data();
  instance_create_info.enabledExtensionCount = required_extensions.size();
  instance_create_info.ppEnabledExtensionNames = required_extensions.data();

  VkResult result = vkCreateInstance(&instance_create_info, vk_allocation, &vk_instance);
  if (result != VK_SUCCESS)
    throw exception("failed to create instance [%s]", util::get_result_string(result));

  surface->vk_create_surface(vk_instance, vk_allocation, &vk_surface);
  return 0;
}

int me::vulkan::Vulkan::cleanup_instance()
{
  vkDestroyInstance(vk_instance, vk_allocation);
  return 0;
}
