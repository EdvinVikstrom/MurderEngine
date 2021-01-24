#include "Vulkan.hpp"
#include "Util.hpp"
#include "lme/array_proxy.hpp"

#include <lme/vector.hpp>
#include <lme/algorithm.hpp>

struct PhysicalDeviceInfo {
  VkPhysicalDevice vk_physical_device;
  VkPhysicalDeviceProperties vk_physical_device_properties;
  VkPhysicalDeviceFeatures vk_physical_device_features;
  uint32_t compute_queue_index = UINT32_MAX;
  uint32_t graphics_queue_index = UINT32_MAX;
  uint32_t present_queue_index = UINT32_MAX;
  uint32_t transfer_queue_index = UINT32_MAX;
};

static int get_suitable_physical_devices(
    VkInstance 							instance,
    VkSurfaceKHR 						surface,
    const me::array_proxy<VkPhysicalDevice, uint32_t> 		&physical_devices,
    const me::array_proxy<const char*, uint32_t>		&required_extensions,
    const me::array_proxy<const char*, uint32_t>		&required_layers,
    uint32_t 							&suitable_physical_device_count,
    PhysicalDeviceInfo*						suitable_physical_devices
    );

static int pick_physical_device(
    me::Logger 							&logger,
    const me::array_proxy<PhysicalDeviceInfo, uint32_t>		&physical_devices,
    uint32_t 							&physical_device_index
    );


int me::vulkan::Vulkan::setup_device(const DeviceInfo &device_info, Device &_device)
{
  uint32_t physical_device_count;
  vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, nullptr);
  VkPhysicalDevice physical_devices[physical_device_count];
  vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, physical_devices);

  PhysicalDeviceInfo suitable_physical_devices[physical_device_count];
  uint32_t suitable_physical_device_count;
  get_suitable_physical_devices(vk_instance, vk_surface, {physical_device_count, physical_devices},
      required_device_extensions, required_device_layers, suitable_physical_device_count, suitable_physical_devices);

  uint32_t physical_device_index;
  pick_physical_device(logger, {suitable_physical_device_count, suitable_physical_devices}, physical_device_index);

  vk_physical_device = suitable_physical_devices[physical_device_index].vk_physical_device;
  queue_family_indices.compute_queue = suitable_physical_devices[physical_device_index].compute_queue_index;
  queue_family_indices.graphics_queue = suitable_physical_devices[physical_device_index].graphics_queue_index;
  queue_family_indices.present_queue = suitable_physical_devices[physical_device_index].present_queue_index;
  queue_family_indices.transfer_queue = suitable_physical_devices[physical_device_index].transfer_queue_index;


  uint32_t unique_queue_families[3];
  uint32_t unique_queue_family_count;

  unique_queue_families[0] = queue_family_indices.compute_queue;
  unique_queue_families[1] = queue_family_indices.graphics_queue;
  unique_queue_families[2] = queue_family_indices.transfer_queue;
  algorithm::unique_array({3, unique_queue_families}, unique_queue_families, unique_queue_family_count);

  float queue_priorities[1] = {1.0F};
  VkDeviceQueueCreateInfo device_queue_create_infos[unique_queue_family_count];
  for (uint32_t i = 0; i < unique_queue_family_count; i++)
  {
    device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_infos[i].pNext = nullptr;
    device_queue_create_infos[i].flags = 0;
    device_queue_create_infos[i].queueFamilyIndex = unique_queue_families[i];
    device_queue_create_infos[i].queueCount = unique_queue_family_count;
    device_queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  /* === Creating a logical device === */
  VkDeviceCreateInfo device_create_info = { };
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = unique_queue_family_count;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.enabledLayerCount = required_device_layers.size();
  device_create_info.ppEnabledLayerNames = required_device_layers.data();
  device_create_info.enabledExtensionCount = required_device_extensions.size();
  device_create_info.ppEnabledExtensionNames = required_device_extensions.data();
  device_create_info.pEnabledFeatures = nullptr;

  VkResult result = vkCreateDevice(vk_physical_device, &device_create_info, vk_allocation, &vk_device);
  if (result != VK_SUCCESS)
    throw exception("failed to create device [%s]", util::get_result_string(result));

  vkGetDeviceQueue(vk_device, queue_family_indices.compute_queue, 0, &queue_families.compute_queue);
  vkGetDeviceQueue(vk_device, queue_family_indices.graphics_queue, 0, &queue_families.graphics_queue);
  vkGetDeviceQueue(vk_device, queue_family_indices.present_queue, 0, &queue_families.present_queue);
  vkGetDeviceQueue(vk_device, queue_family_indices.transfer_queue, 0, &queue_families.transfer_queue);

  _device = 0;
  return 0;
}

int me::vulkan::Vulkan::cleanup_device()
{
  vkDestroyDevice(vk_device, vk_allocation);
  return 0;
}


int get_suitable_physical_devices(
    VkInstance 							instance,
    VkSurfaceKHR 						surface,
    const me::array_proxy<VkPhysicalDevice, uint32_t> 		&physical_devices,
    const me::array_proxy<const char*, uint32_t>		&required_extensions,
    const me::array_proxy<const char*, uint32_t>		&required_layers,
    uint32_t 							&suitable_physical_device_count,
    PhysicalDeviceInfo*						suitable_physical_devices
    )
{
 
  /* === Find suitable physical devices === */
  for (uint32_t i = 0; i < physical_devices.size(); i++)
  {
    VkPhysicalDeviceProperties physical_device_properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);

    VkPhysicalDeviceFeatures physical_device_features;
    vkGetPhysicalDeviceFeatures(physical_devices[i], &physical_device_features);

    /* getting supported device extensions */
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_devices[i], nullptr, &extension_count, nullptr);
    VkExtensionProperties extensions[extension_count];
    vkEnumerateDeviceExtensionProperties(physical_devices[i], nullptr, &extension_count, extensions);

    /* getting supported device layers */
    uint32_t layer_count;
    vkEnumerateDeviceLayerProperties(physical_devices[i], &layer_count, nullptr);
    VkLayerProperties layers[layer_count];
    vkEnumerateDeviceLayerProperties(physical_devices[i], &layer_count, layers);

    /* getting device queue families */
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, nullptr);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_families);

    /* get required queue family indices */
    uint32_t compute_queue_index;
    uint32_t graphics_queue_index;
    uint32_t present_queue_index;
    uint32_t transfer_queue_index;
    for (uint32_t j = 0; j < queue_family_count; j++)
    {
      /* compute queue */
      if (queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
	compute_queue_index = j;

      /* graphics queue */
      if (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	graphics_queue_index = j;

      /* transfer queue */
      if (queue_families[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
	transfer_queue_index = j;

      /* present queue */
      VkBool32 present_support = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices[i], j, surface, &present_support);
      if (present_support)
	present_queue_index = j;
    }


    /* === check if physical device is suitable === */
    /* check if physical device has the required extensions */
    if (!me::vulkan::util::has_required_extensions({extension_count, extensions}, required_extensions))
      continue;

    /* check if physical device has the required layers */
    if (!me::vulkan::util::has_required_layers({layer_count, layers}, required_layers))
      continue;

    /* check if all the required queue families was found */
    if (compute_queue_index == UINT32_MAX || graphics_queue_index == UINT32_MAX ||
	present_queue_index == UINT32_MAX || transfer_queue_index == UINT32_MAX)
      continue;

    /* if the device was suitable; add the physical device to the 'suitable devices' array */
    suitable_physical_devices[suitable_physical_device_count++] = PhysicalDeviceInfo{
      .vk_physical_device = physical_devices[i],
      .vk_physical_device_properties = physical_device_properties,
      .vk_physical_device_features = physical_device_features,
      .compute_queue_index = compute_queue_index,
      .graphics_queue_index = graphics_queue_index,
      .present_queue_index = present_queue_index,
      .transfer_queue_index = transfer_queue_index
    };
  }
  return 0;
}

int pick_physical_device(
    me::Logger 							&logger,
    const me::array_proxy<PhysicalDeviceInfo, uint32_t>		&physical_devices,
    uint32_t 							&physical_device_index
    )
{
  physical_device_index = 0;

  /* ask the user what physical device(s) to use if there is more than 1 physical device */
  if (physical_devices.size() > 1)
  {
    const char* options[physical_devices.size()];
    for (uint32_t i = 0; i < physical_devices.size();i ++)
      options[i] = physical_devices[i].vk_physical_device_properties.deviceName;

    physical_device_index = logger.q_choose({physical_devices.size(), options}, physical_device_index,
	"multiple suitable GPUs found [%u]", physical_devices.size());
  }
  return 0;
}
