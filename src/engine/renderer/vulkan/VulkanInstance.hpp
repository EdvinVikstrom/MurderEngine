#include "Vulkan.inc"

/* <--- SETUP ---> */
int me::Vulkan::setup_extensions()
{
  logger.debug("> SETUP_EXTENSIONS");

  /* instance extensions */
#ifndef NDEBUG
  required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  required_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

  /* get required surface extensions */
  uint32_t surface_extension_count;
  const char** surface_extensions = me_surface->vk_get_required_surface_extensions(surface_extension_count);
  for (uint32_t i = 0; i < surface_extension_count; i++)
    required_extensions.push_back(surface_extensions[i]);

  /* check if supported */
  uint32_t extension_property_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_property_count, nullptr);
  VkExtensionProperties extension_properties[extension_property_count];
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_property_count, extension_properties);

  if (!has_extensions({extension_property_count, extension_properties}, required_extensions))
    throw exception("required extensions not supported");
  return 0;
}

int me::Vulkan::setup_layers()
{
  logger.debug("> SETUP_LAYERS");

#ifndef NDEBUG
  required_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

  /* check if supported */
  uint32_t layer_property_count;
  vkEnumerateInstanceLayerProperties(&layer_property_count, nullptr);
  VkLayerProperties layer_properties[layer_property_count];
  vkEnumerateInstanceLayerProperties(&layer_property_count, layer_properties);

  if (!has_layers({layer_property_count, layer_properties}, required_layers))
    throw exception("required layers not supported");
  return 0;
}

int me::Vulkan::setup_instance(const EngineInfo* engine_info)
{
  logger.debug("> SETUP_INSTANCE");

  VkApplicationInfo application_info = { };
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pNext = nullptr;
  application_info.pApplicationName = engine_info->application_info.name;
  application_info.applicationVersion = engine_info->application_info.version;
  application_info.pEngineName = ME_ENGINE_NAME;
  application_info.engineVersion = VK_MAKE_VERSION(ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo instance_create_info = { };
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = 0;
  instance_create_info.pApplicationInfo = &application_info;
  instance_create_info.enabledLayerCount = (uint32_t) required_layers.size();
  instance_create_info.ppEnabledLayerNames = required_layers.data();
  instance_create_info.enabledExtensionCount = (uint32_t) required_extensions.size();
  instance_create_info.ppEnabledExtensionNames = required_extensions.data();

  VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_info.instance);
  if (result != VK_SUCCESS)
    throw exception("failed to create instance [%s]", vk_utils_result_string(result));
  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_instance()
{
  vkDestroyInstance(instance_info.instance, nullptr);
  return 0;
}
