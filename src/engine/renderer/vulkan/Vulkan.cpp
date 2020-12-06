#include "Vulkan.hpp"
#include "Util.hpp"
#include <vulkan/vulkan_core.h>

#include <map>
#include <cmath>

me::Vulkan::Vulkan(const MurderEngine* engine, const Surface &surface)
  : Renderer(engine, "vulkan"), surface(surface)
{
}

int me::Vulkan::initialize()
{
  /* make a logger */
  logger = engine->get_logger().child("Vulkan");
  logger->trace_all();

  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);


  AppConfig app_config = engine->get_app_config();
  VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = app_config.name,
    .applicationVersion = app_config.version,
    .pEngineName = ME_ENGINE_NAME,
    .engineVersion = VK_MAKE_VERSION(
	ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH)
  };

  VkInstanceCreateInfo instance_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pApplicationInfo = &app_info,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = (uint32_t) surface.get_extensions().size(),
    .ppEnabledExtensionNames = surface.get_extensions().data()
  };

  VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);
  if (result != VK_SUCCESS)
    throw Exception(logger->get_prefix(), true, "failed to create instance [%s]", get_vulkan_result_string(result));

  get_physical_device(VK_QUEUE_GRAPHICS_BIT, extensions, { }, physical_device_info);
  if (physical_device_info.device == VK_NULL_HANDLE)
    throw Exception(logger->get_prefix(), true, "no suitable GPU found");

  create_device(physical_device_info, device);
  create_command_pool(physical_device_info, device, command_pool);

  return 0;
}

int me::Vulkan::tick()
{
  return 0;
}

int me::Vulkan::terminate()
{
  vkDestroyInstance(instance, nullptr);
  return 0;
}

int me::Vulkan::get_physical_device_queue_family(const VkPhysicalDevice physical_device, const int required_flags, uint32_t &family_index)
{
  uint32_t queue_family_property_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);

  VkQueueFamilyProperties queue_family_properties[queue_family_property_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties);

  /* check if any family queue has the required flags */
  for (uint32_t i = 0; i < queue_family_property_count; i++)
  {
    if (queue_family_properties[i].queueCount > 0)
    {
      if ((queue_family_properties[i].queueFlags & required_flags) == required_flags)
      {
	family_index = i;
	return 1;
      }
    }
  }
  return 0;
}

int me::Vulkan::get_physical_device_extensions(const VkPhysicalDevice physical_device,
    const std::vector<const char*> &required_extensions, std::vector<VkExtensionProperties> &extensions)
{
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

  extensions.resize(extension_count);
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());

  for (const char* required : required_extensions)
  {
    bool found = false;
    for (VkExtensionProperties extension : extensions)
    {
      if (strcmp(required, extension.extensionName) == 0)
      {
	found = true;
	break;
      }
    }

    if (!found)
      return 0;
  }
  return 1;
}

int me::Vulkan::get_physical_device_features(const VkPhysicalDevice physical_device,
    const std::vector<const char*> &required_features, VkPhysicalDeviceFeatures &features)
{
  vkGetPhysicalDeviceFeatures(physical_device, &features);
  return 1;
}

int me::Vulkan::get_physical_device(const int required_flags,
    const std::vector<const char*> &required_extensions, const std::vector<const char*> &required_features, PhysicalDeviceInfo &physical_device_info)
{
  /* get the count of physical devices */
  uint32_t physical_device_count;
  vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);

  /* populate physical device array */
  VkPhysicalDevice physical_devices[physical_device_count];
  vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);


  for (uint32_t i = 0; i < physical_device_count; i++)
  {
    physical_device_info.device = physical_devices[i];

    /* get physical device properties */
    VkPhysicalDeviceProperties physical_device_properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);

    logger->debug("found device[%s]", physical_device_properties.deviceName);

    /* check and get physical device features */
    VkPhysicalDeviceFeatures physical_device_features;
    if (!get_physical_device_features(physical_devices[i], required_features, physical_device_features))
      continue;

    /* check and get physical device extensions */
    std::vector<VkExtensionProperties> physical_device_extensions;
    if (!get_physical_device_extensions(physical_devices[i], required_extensions, physical_device_extensions))
      continue;

    /* check and get physical device family queue flags */
    uint32_t family_index;
    if (!get_physical_device_queue_family(physical_devices[i], required_flags, family_index))
      continue;

    if (physical_device_info.device == VK_NULL_HANDLE || physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      physical_device_info.device = physical_devices[i];
      physical_device_info.properties = physical_device_properties;
      physical_device_info.features = physical_device_features;
      physical_device_info.extensions = physical_device_extensions;
      physical_device_info.queue_family_indices.graphics = family_index;
    }
  }
  return 0;
}


int me::Vulkan::create_device(const PhysicalDeviceInfo &physical_device_info, VkDevice &device)
{
  char const* extension_names[physical_device_info.extensions.size()];
  for (uint32_t i = 0; i < physical_device_info.extensions.size(); i++)
    extension_names[i] = physical_device_info.extensions.at(i).extensionName;


  float queue_priorities[] { 1.0F };
  uint32_t queue_count = 1;
  VkDeviceQueueCreateInfo queue_infos[queue_count];
  queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_infos[0].pNext = nullptr;
  queue_infos[0].flags = 0;
  queue_infos[0].queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
  queue_infos[0].queueCount = 1;
  queue_infos[0].pQueuePriorities = queue_priorities;

  VkDeviceCreateInfo device_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueCreateInfoCount = queue_count,
    .pQueueCreateInfos = queue_infos,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = (uint32_t) physical_device_info.extensions.size(),
    .ppEnabledExtensionNames = extension_names,
    .pEnabledFeatures = &physical_device_info.features
  };

  VkResult result = vkCreateDevice(physical_device_info.device, &device_info, nullptr, &device);
  if (result != VK_SUCCESS)
    throw Exception(logger->get_prefix(), true, "failed to create device [%s]", get_vulkan_result_string(result));

  return 0;
}


int me::Vulkan::create_command_pool(const PhysicalDeviceInfo &physical_device_info, const VkDevice device, VkCommandPool &command_pool)
{
  VkCommandPoolCreateInfo command_pool_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value()
  };

  VkResult result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
  if (result != VK_SUCCESS)
    throw Exception(logger->get_prefix(), true, "failed to create command pool [%s]", get_vulkan_result_string(result));

  return 0;
}
