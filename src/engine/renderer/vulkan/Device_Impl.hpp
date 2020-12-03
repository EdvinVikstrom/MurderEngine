#include "VulkanRenderer.hpp"

VkPhysicalDevice& me::VulkanRenderer::get_primary_device(uint32_t count, VkPhysicalDevice* devices)
{
  return devices[0];
}

int me::VulkanRenderer::init_devices()
{
  /* get physical devices count */
  uint32_t pdev_count;
  VkResult result = vkEnumeratePhysicalDevices(instance, &pdev_count, nullptr);
  if (result != VK_SUCCESS)
    vk_error(result);

  if (pdev_count == 0)
    throw Exception(logger->get_prefix(), true, "no physical devices found");

  logger->debug("found %u physical device(s)", pdev_count);

  /* get physical device list */
  VkPhysicalDevice physical_devices[pdev_count];
  result = vkEnumeratePhysicalDevices(instance, &pdev_count, physical_devices);
  if (result != VK_SUCCESS)
    vk_error(result);

  physical_device = get_primary_device(pdev_count, physical_devices);


  /* get the device properties */
  vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

  /* get supported features */
  vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

  /* get queue family properties */
  uint32_t family_queue_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_queue_count, nullptr);

  VkQueueFamilyProperties queue_family_properties[family_queue_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_queue_count, queue_family_properties);

  for (uint32_t i = 0; i < family_queue_count; i++)
  {
    VkQueueFamilyProperties queue_family = queue_family_properties[i];
    VkQueueFlags &queue_family_flags = queue_family.queueFlags;

    /* graphics queue */
    if (queue_family_flags & VK_QUEUE_GRAPHICS_BIT)
      queue_family_indices.graphics_family = i;
    /* compute queue */
    else if (queue_family_flags & VK_QUEUE_COMPUTE_BIT)
      queue_family_indices.compute_family = i;
    /* transfer queue */
    else if (queue_family_flags & VK_QUEUE_TRANSFER_BIT)
      queue_family_indices.transfer_family = i;
  }


  logger->debug("using device[%s](%u) api-%u.%u.%u", physical_device_properties.deviceName,
      physical_device_properties.deviceID,
      VK_VERSION_MAJOR(physical_device_properties.apiVersion), 
      VK_VERSION_MINOR(physical_device_properties.apiVersion), 
      VK_VERSION_PATCH(physical_device_properties.apiVersion));


  /* create device groups */
  uint32_t group_count;
  result = vkEnumeratePhysicalDeviceGroups(instance, &group_count, nullptr);
  if (result != VK_SUCCESS)
    vk_error(result);

  physical_devices_group_properties.resize(group_count, { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES });
  result = vkEnumeratePhysicalDeviceGroups(instance, &group_count, physical_devices_group_properties.data());
  if (result != VK_SUCCESS)
    vk_error(result);
  /* -------------------- */


  /* create logical device */

  /* NOTE: you can add more queues */
  uint32_t queue_count = 1;
  VkDeviceQueueCreateInfo queues[queue_count];

  /* graphics queue create info */
  queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queues[0].pNext = nullptr;
  queues[0].flags = 0; /* set this if using protected memory */
  queues[0].queueFamilyIndex = queue_family_indices.graphics_family.value();
  queues[0].queueCount = 1;
  queues[0].pQueuePriorities = new float[1] { 1.0F };
  this->queues.graphics_queues.resize(1);


  VkDeviceCreateInfo device_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueCreateInfoCount = queue_count,
    .pQueueCreateInfos = queues,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = nullptr,
    .pEnabledFeatures = nullptr /*&physical_device.features.features*/
  };

  /* TODO: handle lost devices */
  result = vkCreateDevice(physical_device, &device_info, &alloc_callbacks, &device);
  if (result != VK_SUCCESS)
    vk_error(result);

  /* get the queues from device */
  for (uint32_t j = 0; j < this->queues.graphics_queues.size(); j++)
    vkGetDeviceQueue(device, queue_family_indices.graphics_family.value(), j, &this->queues.graphics_queues.at(j));
  return 0;
}

int me::VulkanRenderer::free_devices()
{
  vkDeviceWaitIdle(device); /* wait if any work is active */
  vkDestroyDevice(device, &alloc_callbacks);
  return 0;
}
