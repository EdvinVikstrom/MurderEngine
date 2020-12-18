#include "Vulkan.hpp"

#include "../../util/vk_utils.hpp"

#include <cmath>
#include <cstring>
#include <lme/map.hpp>
#include <algorithm>
#include <vulkan/vulkan_core.h>

me::Vulkan::Vulkan(const MurderEngine* engine, const VulkanSurface &surface_instance)
  : Renderer(engine, "vulkan"), surface_instance(surface_instance)
{
}

int me::Vulkan::initialize()
{
  /* make a logger */
  logger = engine->get_logger().child("Vulkan");

  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);


  uint32_t surface_extension_count;
  const char** surface_extensions = surface_instance.get_extensions(surface_extension_count);

  /* create a vulkan instance */
  AppConfig app_config = engine->get_app_config();
  VkApplicationInfo app_info = { };
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = app_config.name;
  app_info.applicationVersion = app_config.version;
  app_info.pEngineName = ME_ENGINE_NAME;
  app_info.engineVersion = VK_MAKE_VERSION(
	ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo instance_info = { };
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pNext = nullptr;
  instance_info.flags = 0;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledLayerCount = 0;
  instance_info.ppEnabledLayerNames = nullptr;
  instance_info.enabledExtensionCount = surface_extension_count;
  instance_info.ppEnabledExtensionNames = surface_extensions;

  VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);
  if (result != VK_SUCCESS)
    throw exception("failed to create instance [%s]", vk_utils_result_string(result));


  /* create window surface */
  surface_instance.create_surface(instance, nullptr, &surface);

  /* get a physical device */
  get_physical_device(VK_QUEUE_GRAPHICS_BIT, extensions, { }, physical_device_info);
  if (physical_device_info.device == VK_NULL_HANDLE)
    throw exception("no suitable GPU found");

  /* create a logical device */
  create_device(device);

  /* create a command pool */
  create_command_pool(command_pool);

  /* init swapchain */
  create_swapchain(swapchain_info);

  /* create pipeline layout */
  create_pipeline_layout(pipeline_layout_info);

  /* create viewports */
  viewport_info.viewports.push_back(VkViewport{
      .x = 0.0F, .y = 0.0F,
      .width = (float) swapchain_info.extent.width, .height = (float) swapchain_info.extent.height,
      .minDepth = 0.0F, .maxDepth = 1.0F});
  viewport_info.scissors.push_back(VkRect2D{
      .offset = {0, 0},
      .extent = swapchain_info.extent});
  create_viewports(viewport_info);

  /* create rasterizer */
  create_rasterizer(rasterizer_info);

  /* create multisampling */
  create_multisampling(multisampling_info);

  /* create color blend attachment */
  color_blend_info.pipeline_color_blend_attachment_states.push_back(VkPipelineColorBlendAttachmentState{
      VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT});
  create_color_blend(color_blend_info);

  /* create dynamic */
  dynamic_info.dynamics.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  dynamic_info.dynamics.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
  create_dynamic(dynamic_info);

  return 0;
}

int me::Vulkan::tick(const Context context)
{
  return 0;
}

int me::Vulkan::terminate()
{
  for (VkImageView view : swapchain_info.image_views)
    vkDestroyImageView(device, view, nullptr);

  vkDestroyPipelineLayout(device, pipeline_layout_info.pipeline_layout, nullptr);
  vkDestroySwapchainKHR(device, swapchain_info.swapchain, nullptr);
  vkDestroyCommandPool(device, command_pool, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
  return 0;
}

int me::Vulkan::get_physical_device_queue_family(const VkPhysicalDevice physical_device, const int required_flags, uint32_t &family_index)
{
  /* get queue family properties from physical device */
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
    const vector<const char*> &required_extensions, vector<VkExtensionProperties> &extensions)
{
  /* get extension properties from physical device */
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

  extensions.resize(extension_count);
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());


  /* check if physical device has required extensions */
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
    const vector<const char*> &required_features, VkPhysicalDeviceFeatures &features)
{
  vkGetPhysicalDeviceFeatures(physical_device, &features);
  return 1;
}

int me::Vulkan::get_physical_device(const int required_flags,
    const vector<const char*> &required_extensions, const vector<const char*> &required_features, PhysicalDeviceInfo &physical_device_info)
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
    vector<VkExtensionProperties> physical_device_extensions;
    if (!get_physical_device_extensions(physical_devices[i], required_extensions, physical_device_extensions))
      continue;

    /* check and get physical device family queue flags */
    uint32_t family_index;
    if (!get_physical_device_queue_family(physical_devices[i], required_flags, family_index))
      continue;


    /* choose device */
    if (physical_device_info.device == VK_NULL_HANDLE || physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      physical_device_info.device = physical_devices[i];
      physical_device_info.properties = physical_device_properties;
      physical_device_info.features = physical_device_features;
      physical_device_info.extensions = physical_device_extensions;
      physical_device_info.queue_family_indices.graphics.assign(family_index);
    }
  }
  return 0;
}


int me::Vulkan::create_device(VkDevice &device)
{
  /* get the extension names from a VkExtensionProperties array */
  char const* extension_names[physical_device_info.extensions.size()];
  for (uint32_t i = 0; i < physical_device_info.extensions.size(); i++)
    extension_names[i] = physical_device_info.extensions.at(i).extensionName;


  /* graphics queue info */
  float queue_priorities[] { 1.0F };
  uint32_t queue_count = 1;
  VkDeviceQueueCreateInfo queue_infos[queue_count];
  queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_infos[0].pNext = nullptr;
  queue_infos[0].flags = 0;
  queue_infos[0].queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
  queue_infos[0].queueCount = 1;
  queue_infos[0].pQueuePriorities = queue_priorities;

  /* create logical device */
  VkDeviceCreateInfo device_create_info = { };
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = queue_count;
  device_create_info.pQueueCreateInfos = queue_infos;
  device_create_info.enabledLayerCount = 0;
  device_create_info.ppEnabledLayerNames = nullptr;
  device_create_info.enabledExtensionCount = (uint32_t) physical_device_info.extensions.size();
  device_create_info.ppEnabledExtensionNames = extension_names;
  device_create_info.pEnabledFeatures = &physical_device_info.features;

  VkResult result = vkCreateDevice(physical_device_info.device, &device_create_info, nullptr, &device);
  if (result != VK_SUCCESS)
    throw exception("failed to create device [%s]", vk_utils_result_string(result));

  return 0;
}


int me::Vulkan::create_command_pool(VkCommandPool &command_pool)
{
  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();

  VkResult result = vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));

  return 0;
}


int me::Vulkan::get_surface_format(const vector<VkSurfaceFormatKHR> &formats, VkSurfaceFormatKHR &format)
{
#define FORMAT VK_FORMAT_B8G8R8A8_UNORM
#define COLOR_SPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR

  if (formats.size() == 1 && formats.at(0).format == VK_FORMAT_UNDEFINED)
  {
    format = { FORMAT, COLOR_SPACE };
    return 1;
  }

  for (uint32_t i = 0; i < formats.size(); i++)
  {
    if (formats.at(i).format == FORMAT && formats.at(i).colorSpace == COLOR_SPACE)
    {
      format = formats.at(i);
      return 0;
    }
  }

  if (formats.size() > 0)
  {
    format = formats.at(0);
    return 1;
  }

#undef FORMAT
#undef COLOR_SPACE

  throw exception("no surface formats was provided");
}

int me::Vulkan::get_present_mode(const vector<VkPresentModeKHR> &modes, VkPresentModeKHR &mode)
{
  for (uint32_t i = 0; i < modes.size(); i++)
  {
    if (modes.at(i) == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      mode = modes.at(i);
      return 0;
    }
  }

  mode = VK_PRESENT_MODE_FIFO_KHR;
  return 1;
}

int me::Vulkan::get_extent(const Surface &surface_instance, const VkSurfaceCapabilitiesKHR surface_capabilities, VkExtent2D &extent)
{
  if (surface_capabilities.currentExtent.width != UINT32_MAX)
  {
    extent = surface_capabilities.currentExtent;
    return 0;
  }

  /* get the size of the surface */
  surface_instance.get_size(extent.width, extent.height);

  /* if extent.width/height is more than max image width/height; set the extent.width/height to max image width/height */
  extent.width = std::min(extent.width, surface_capabilities.maxImageExtent.width);
  extent.height = std::min(extent.height, surface_capabilities.maxImageExtent.height);

  /* if extent.width/height is less than min image width/height; set the extent.width/height to min image width/height */
  extent.width = std::max(extent.width, surface_capabilities.minImageExtent.width);
  extent.width = std::max(extent.height, surface_capabilities.minImageExtent.height);
  return 0;
}

int me::Vulkan::create_swapchain(SwapchainInfo &swapchain_info)
{
  /* get physical device surface capabilities */
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_info.device, surface, &surface_info.capabilities);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface capabilities [%s]", vk_utils_result_string(result));


  /* get surface formats */
  uint32_t surface_format_count;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface, &surface_format_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats count [%s]", vk_utils_result_string(result));

  surface_info.formats.resize(surface_format_count);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface, &surface_format_count, surface_info.formats.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats [%s]", vk_utils_result_string(result));


  /* get surface present modes */
  uint32_t present_mode_count;
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface, &present_mode_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes count [%s]", vk_utils_result_string(result));

  surface_info.present_modes.resize(present_mode_count);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface, &present_mode_count, surface_info.present_modes.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes [%s]", vk_utils_result_string(result));


  /* get surface format */
  get_surface_format(surface_info.formats, surface_info.format);

  /* get present mode */
  get_present_mode(surface_info.present_modes, surface_info.present_mode);

  /* get extent */
  get_extent(surface_instance, surface_info.capabilities, surface_info.extent);
  swapchain_info.extent = surface_info.extent;


  uint32_t min_image_count = std::min(surface_info.capabilities.minImageCount + 1,
      surface_info.capabilities.maxImageCount > 0 ? surface_info.capabilities.maxImageCount : -1);

  /* create swapchain */
  VkSwapchainCreateInfoKHR swapchain_create_info = { };
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = nullptr;
  swapchain_create_info.flags = 0;
  swapchain_create_info.surface = surface;
  swapchain_create_info.minImageCount = min_image_count;
  swapchain_create_info.imageFormat = surface_info.format.format;
  swapchain_create_info.imageColorSpace = surface_info.format.colorSpace;
  swapchain_create_info.imageExtent = surface_info.extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.preTransform = surface_info.capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = surface_info.present_mode;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; /* TODO */

  result = vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain_info.swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", vk_utils_result_string(result));


  /* get swapchain images */
  uint32_t image_count;
  result = vkGetSwapchainImagesKHR(device, swapchain_info.swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", vk_utils_result_string(result));

  swapchain_info.images.resize(image_count);
  result = vkGetSwapchainImagesKHR(device, swapchain_info.swapchain, &image_count, swapchain_info.images.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images [%s]", vk_utils_result_string(result));


  /* create swapchain image views */
  swapchain_info.image_views.resize(image_count);
  for (uint32_t i = 0; i < image_count; i++)
  {
    VkImageViewCreateInfo image_view_create_info = { };
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = swapchain_info.images.at(i);
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = surface_info.format.format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device, &image_view_create_info, nullptr, &swapchain_info.image_views.at(i));
    if (result != VK_SUCCESS)
      throw exception("failed to create image view [%s]", vk_utils_result_string(result));
  }

  return 0;
}


int me::Vulkan::create_pipeline_layout(PipelineLayoutInfo &pipeline_layout_info)
{
  pipeline_layout_info.pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.pipeline_layout_create_info.pNext = nullptr;
  pipeline_layout_info.pipeline_layout_create_info.flags = 0;
  pipeline_layout_info.pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_info.pipeline_layout_create_info.pSetLayouts = nullptr;
  pipeline_layout_info.pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pipeline_layout_create_info.pPushConstantRanges = nullptr;

  VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_info.pipeline_layout_create_info, nullptr, &pipeline_layout_info.pipeline_layout);
  if (result != VK_SUCCESS)
    throw exception("failed to create pipeline layout [%s]", vk_utils_result_string(result));
  return 0;
}


int me::Vulkan::create_viewports(ViewportInfo &viewport_info)
{
  viewport_info.pipline_viewport_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.pipline_viewport_stage_create_info.pNext = nullptr;
  viewport_info.pipline_viewport_stage_create_info.flags = 0;
  viewport_info.pipline_viewport_stage_create_info.viewportCount = (uint32_t) viewport_info.viewports.size();
  viewport_info.pipline_viewport_stage_create_info.pViewports = viewport_info.viewports.data();
  viewport_info.pipline_viewport_stage_create_info.scissorCount = (uint32_t) viewport_info.scissors.size();
  viewport_info.pipline_viewport_stage_create_info.pScissors = viewport_info.scissors.data();
  return 0;
}


int me::Vulkan::create_rasterizer(RasterizerInfo &rasterizer_info)
{
  rasterizer_info.pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_info.pipeline_rasterization_state_create_info.pNext = nullptr;
  rasterizer_info.pipeline_rasterization_state_create_info.flags = 0;
  rasterizer_info.pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
  rasterizer_info.pipeline_rasterization_state_create_info.rasterizerDiscardEnable = true;
  rasterizer_info.pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_info.pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_info.pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_info.pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
  rasterizer_info.pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0F;
  rasterizer_info.pipeline_rasterization_state_create_info.depthBiasClamp = 0.0F;
  rasterizer_info.pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0F;
  rasterizer_info.pipeline_rasterization_state_create_info.lineWidth = 1.0F;
  return 0;
}


int me::Vulkan::create_multisampling(MultisamplingInfo &multisampling_info)
{
  multisampling_info.pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.pipeline_multisample_state_create_info.pNext = nullptr;
  multisampling_info.pipeline_multisample_state_create_info.flags = 0;
  multisampling_info.pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_info.pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.pipeline_multisample_state_create_info.minSampleShading = 1.0F;
  multisampling_info.pipeline_multisample_state_create_info.pSampleMask = nullptr;
  multisampling_info.pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
  multisampling_info.pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;
  return 0;
}


int me::Vulkan::create_color_blend(ColorBlendInfo &color_blend_info)
{
  color_blend_info.pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.pipeline_color_blend_state_create_info.pNext = nullptr;
  color_blend_info.pipeline_color_blend_state_create_info.flags = 0;
  color_blend_info.pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_info.pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_info.pipeline_color_blend_state_create_info.attachmentCount = (uint32_t) color_blend_info.pipeline_color_blend_attachment_states.size();
  color_blend_info.pipeline_color_blend_state_create_info.pAttachments = color_blend_info.pipeline_color_blend_attachment_states.data();
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[0] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[1] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[2] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[3] = 0.0F;
  return 0;
}


int me::Vulkan::create_dynamic(DynamicInfo &dynamic_info)
{
  dynamic_info.pipline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_info.pipline_dynamic_state_create_info.pNext = nullptr;
  dynamic_info.pipline_dynamic_state_create_info.flags = 0;
  dynamic_info.pipline_dynamic_state_create_info.dynamicStateCount = (uint32_t) dynamic_info.dynamics.size();
  dynamic_info.pipline_dynamic_state_create_info.pDynamicStates = dynamic_info.dynamics.data();
  return 0;
}


int me::Vulkan::compile_shader(Shader* shader) const
{
  VkShaderModuleCreateInfo shader_create_info = { };
  shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_create_info.pNext = nullptr;
  shader_create_info.flags = 0;
  shader_create_info.codeSize = shader->get_length();
  shader_create_info.pCode = reinterpret_cast<const uint32_t*>(shader->get_data());

  VkShaderModule shader_module;
  VkResult result = vkCreateShaderModule(device, &shader_create_info, nullptr, &shader_module);
  if (result != VK_SUCCESS)
    throw exception("failed to create shader module [%s]", vk_utils_result_string(result));


  VkPipelineShaderStageCreateInfo shader_stage_create_info = { };
  shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_create_info.pNext = nullptr;
  shader_stage_create_info.flags = 0;
  shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  shader_stage_create_info.module = shader_module;
  shader_stage_create_info.pName = shader->get_config().entry_point.c_str();
  shader_stage_create_info.pSpecializationInfo = nullptr;

  temp.pipeline_shader_stage_create_infos.push_back(shader_stage_create_info);
  return 0;
}
