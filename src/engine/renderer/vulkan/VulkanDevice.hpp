#include "Vulkan.inc"

/* <--- SETUP ---> */
int me::Vulkan::setup_device_extensions()
{
  logger.debug("> SETUP_DEVICE_EXTENSIONS");

  required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  return 0;
}

int me::Vulkan::setup_device_layers()
{
  logger.debug("> SETUP_DEVICE_LAYERS");

  return 0;
}

int me::Vulkan::setup_physical_device()
{
  logger.debug("> SETUP_PHYSICAL_DEVICE");

  /* get physical devices */
  uint32_t physical_device_count;
  vkEnumeratePhysicalDevices(instance_info.instance, &physical_device_count, nullptr);
  VkPhysicalDevice physical_devices[physical_device_count];
  vkEnumeratePhysicalDevices(instance_info.instance, &physical_device_count, physical_devices);

  /* get physical device infos */
  PhysicalDeviceInfo physical_device_infos[physical_device_count];
  get_physical_device_infos({physical_device_count, physical_devices}, physical_device_infos);

  for (const PhysicalDeviceInfo &physical_device_info : physical_device_infos)
  {
    logger.debug("found device[%s]", physical_device_info.properties.deviceName);

    /* check required extensions */
    if (!has_extensions(physical_device_info.extensions, required_extensions))
      continue;

    /* check required layers */
    if (!has_layers(physical_device_info.layers, required_device_layers))
      continue;

    /* check required queue family */
    if (!has_queue_families(physical_device_info.queue_family_properties, required_queue_family_properties))
      continue;

    /* choose device */
    if (this->physical_device_info.device == VK_NULL_HANDLE || physical_device_info.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      this->physical_device_info = physical_device_info;
  }
  return 0;
}

int me::Vulkan::setup_logical_device()
{
  logger.debug("> SETUP_LOGICAL_DEVICE");

  /* get unique queue families */
  uint32_t queue_families[2] = {
    physical_device_info.queue_family_indices.graphics.value(),
    physical_device_info.queue_family_indices.present.value()
  };
  set<uint32_t> unique_queue_families(2, queue_families);

  /* device queue create info(s) */
  uint32_t device_queue_count = unique_queue_families.size();
  VkDeviceQueueCreateInfo device_queue_create_infos[device_queue_count];

  /* graphics queue */
  float queue_priorities[1] = {1.0F};
  for (uint32_t i = 0; i < device_queue_count; i++)
  {
    device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_infos[i].pNext = nullptr;
    device_queue_create_infos[i].flags = 0;
    device_queue_create_infos[i].queueFamilyIndex = unique_queue_families.at(i);
    device_queue_create_infos[i].queueCount = device_queue_count;
    device_queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  /* device create info */
  VkDeviceCreateInfo device_create_info = { };
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = device_queue_count;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.enabledLayerCount = (uint32_t) required_device_layers.size();
  device_create_info.ppEnabledLayerNames = required_device_layers.data();
  device_create_info.enabledExtensionCount = (uint32_t) required_device_extensions.size();
  device_create_info.ppEnabledExtensionNames = required_device_extensions.data();
  device_create_info.pEnabledFeatures = &physical_device_info.features;

  VkResult result = vkCreateDevice(physical_device_info.device, &device_create_info, nullptr, &logical_device_info.device);
  if (result != VK_SUCCESS)
    throw exception("failed to create device [%s]", vk_utils_result_string(result));

  vkGetDeviceQueue(logical_device_info.device, physical_device_info.queue_family_indices.graphics.value(), 0, &queue_info.graphics_queue);
  vkGetDeviceQueue(logical_device_info.device, physical_device_info.queue_family_indices.present.value(), 0, &queue_info.present_queue);
  return 0;
}


/* <--- HELPERS ---> */
int me::Vulkan::get_physical_device_infos(const array_proxy<VkPhysicalDevice> &physical_devices, PhysicalDeviceInfo* physical_device_infos)
{
  for (uint32_t i = 0; i < physical_devices.size(); i++)
  {
    PhysicalDeviceInfo &physical_device_info = physical_device_infos[i];
    physical_device_info.device = physical_devices[i];

    vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_info.properties);
    vkGetPhysicalDeviceFeatures(physical_devices[i], &physical_device_info.features);

    /* get physical device queue family properties */
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i],
	&physical_device_info.queue_family_properties.count, nullptr);
    alloc.allocate_array(physical_device_info.queue_family_properties);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i],
	&physical_device_info.queue_family_properties.count, physical_device_info.queue_family_properties.ptr);

    /* get physical device extensions */
    vkEnumerateDeviceExtensionProperties(physical_devices[i], nullptr,
	&physical_device_info.extensions.count, nullptr);
    alloc.allocate_array(physical_device_info.extensions);
    vkEnumerateDeviceExtensionProperties(physical_devices[i], nullptr,
	&physical_device_info.extensions.count, physical_device_info.extensions.ptr);

    find_queue_families(physical_devices[i], surface_info.surface,
	physical_device_info.queue_family_properties, physical_device_info.queue_family_indices);
  }
  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_logical_device()
{
  vkDestroyDevice(logical_device_info.device, nullptr);
  return 0;
}
