#include "Vulkan.hpp"

#include "../../util/vk_utils.hpp"

#include <lme/map.hpp>
#include <lme/math/math.hpp>
#include <lme/string.hpp>
#include <vulkan/vulkan_core.h>

me::Vulkan::Vulkan(const MurderEngine* engine, const VulkanSurface &me_surface)
  : Renderer(engine, "vulkan"), me_surface(me_surface)
{
}

int me::Vulkan::initialize()
{
  /* make a logger */
  logger = engine->get_logger().child("Vulkan");
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  setup_instance();
  setup_physical_device();
  setup_surface();
  setup_logical_device();
  setup_command_pool();
  setup_swapchain();
  setup_pipeline_layout();
  setup_render_pass();

  setup_viewports();
  setup_rasterizer();
  setup_multisampling();
  setup_color_blend();
  setup_dynamic();

  return 0;
}

int me::Vulkan::terminate()
{
  for (VkImageView view : swapchain_info.image_views)
    vkDestroyImageView(logical_device_info.device, view, nullptr);

  vkDestroyPipelineLayout(logical_device_info.device, pipeline_layout_info.pipeline_layout, nullptr);
  vkDestroySwapchainKHR(logical_device_info.device, swapchain_info.swapchain, nullptr);
  vkDestroyCommandPool(logical_device_info.device, command_pool_info.command_pool, nullptr);
  vkDestroyDevice(logical_device_info.device, nullptr);
  vkDestroySurfaceKHR(instance_info.instance, surface_info.surface, nullptr);
  vkDestroyInstance(instance_info.instance, nullptr);
  return 0;
}

int me::Vulkan::tick(const Context context)
{
  return 0;
}


int me::Vulkan::setup_instance()
{
  InstanceCreateInfo instance_create_info = { };

  uint32_t extension_count;
  const char** extensions = me_surface.get_extensions(extension_count);
  for (uint32_t i = 0; i < extension_count; i++)
    instance_create_info.extensions.push_back(extensions[i]);

  AppConfig app_config = engine->get_app_config();
  instance_create_info.application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  instance_create_info.application_info.pNext = nullptr;
  instance_create_info.application_info.pApplicationName = app_config.name;
  instance_create_info.application_info.applicationVersion = app_config.version;
  instance_create_info.application_info.pEngineName = ME_ENGINE_NAME;
  instance_create_info.application_info.engineVersion = VK_MAKE_VERSION(
	ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  instance_create_info.info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.info.pNext = nullptr;
  instance_create_info.info.flags = 0;
  instance_create_info.info.pApplicationInfo = &instance_create_info.application_info;
  instance_create_info.info.enabledLayerCount = 0;
  instance_create_info.info.ppEnabledLayerNames = nullptr;
  instance_create_info.info.enabledExtensionCount = (uint32_t) instance_create_info.extensions.size();
  instance_create_info.info.ppEnabledExtensionNames = instance_create_info.extensions.data();
  return create_instance(instance_create_info, instance_info);
}

int me::Vulkan::setup_physical_device()
{
  PhysicalDeviceCreateInfo physical_device_create_info = { };

  vector<VkPhysicalDevice> physical_devices;
  get_physical_devices(physical_devices);

  physical_device_create_info.physical_device_infos.resize(physical_devices.size());
  for (uint32_t i = 0; i < physical_devices.size(); i++)
  {
    const VkPhysicalDevice physical_device = physical_devices.at(i);
    get_physical_device_properties(physical_device, physical_device_create_info.physical_device_infos.at(i).properties);
    get_physical_device_extensions(physical_device, physical_device_create_info.physical_device_infos.at(i).extensions);
    get_physical_device_features(physical_device, physical_device_create_info.physical_device_infos.at(i).features);
    get_physical_device_queue_family(physical_device, physical_device_create_info.physical_device_infos.at(i).queue_family_indices);
  }

  /* create physical device */
  physical_device_create_info.required_flags = VK_QUEUE_GRAPHICS_BIT;
  physical_device_create_info.required_extensions = extensions;
  return create_physical_device(physical_device_create_info, physical_device_info);
}

int me::Vulkan::setup_surface()
{
  SurfaceCreateInfo surface_create_info = { };

  me_surface.create_surface(instance_info.instance, nullptr, &surface_info.surface);
  get_surface_capabilities(surface_info.surface, surface_info.capabilities);

  get_surface_present_modes(surface_info.surface, surface_create_info.present_modes);
  get_surface_formats(surface_info.surface, surface_create_info.formats);
  return create_surface(surface_create_info, surface_info);
}

int me::Vulkan::setup_logical_device()
{
  LogicalDeviceCreateInfo logical_device_create_info = { };

  logical_device_create_info.device_queue_create_infos.push_back(VkDeviceQueueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value(),
      .queueCount = 1,
      .pQueuePriorities = new float[1] {1.0F}});
  return create_logical_device(logical_device_create_info, logical_device_info);
}

int me::Vulkan::setup_command_pool()
{
  CommandPoolCreateInfo command_pool_create_info = { };

  command_pool_create_info.device = logical_device_info.device;
  return create_command_pool(command_pool_create_info, command_pool_info);
}

int me::Vulkan::setup_swapchain()
{
  SwapchainCreateInfo swapchain_create_info = { };

  swapchain_info.image_format = surface_info.format.format;
  swapchain_info.image_color_space = surface_info.format.colorSpace;
  swapchain_info.image_extent = surface_info.extent;

  swapchain_create_info.device = logical_device_info.device;
  swapchain_create_info.min_image_count = math::min(surface_info.capabilities.minImageCount + 1,
      surface_info.capabilities.maxImageCount > 0 ? surface_info.capabilities.maxImageCount : -1);
  swapchain_create_info.surface = surface_info.surface;
  swapchain_create_info.pre_transform = surface_info.capabilities.currentTransform;
  swapchain_create_info.present_mode = surface_info.present_mode;
  return create_swapchain(swapchain_create_info, swapchain_info);
}

int me::Vulkan::setup_pipeline_layout()
{
  PipelineLayoutCreateInfo pipeline_layout_create_info = { };

  pipeline_layout_create_info.device = logical_device_info.device;
  return create_pipeline_layout(pipeline_layout_create_info, pipeline_layout_info);
}

int me::Vulkan::setup_render_pass()
{
  RenderPassCreateInfo render_pass_create_info = { };

  //render_pass_create_info.attachment_description.format = swapchain_info.
  return create_render_pass(render_pass_create_info, render_pass_info);
}


int me::Vulkan::setup_viewports()
{
  viewport_info.viewports.push_back(VkViewport{
      .x = 0.0F, .y = 0.0F,
      .width = (float) swapchain_info.image_extent.width, .height = (float) swapchain_info.image_extent.height,
      .minDepth = 0.0F, .maxDepth = 1.0F});
  viewport_info.scissors.push_back(VkRect2D{
      .offset = {0, 0},
      .extent = swapchain_info.image_extent});
  return create_viewports(viewport_info);
}

int me::Vulkan::setup_rasterizer()
{
  return create_rasterizer(rasterizer_info);
}

int me::Vulkan::setup_multisampling()
{
  return create_multisampling(multisampling_info);
}

int me::Vulkan::setup_color_blend()
{
  color_blend_info.pipeline_color_blend_attachment_states.push_back(VkPipelineColorBlendAttachmentState{
      VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT});
  return create_color_blend(color_blend_info);
}

int me::Vulkan::setup_dynamic()
{
  dynamic_info.dynamics.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  dynamic_info.dynamics.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
  return create_dynamic(dynamic_info);
}


int me::Vulkan::create_instance(InstanceCreateInfo &instance_create_info, InstanceInfo &instance_info)
{
  VkResult result = vkCreateInstance(&instance_create_info.info, nullptr, &instance_info.instance);
  if (result != VK_SUCCESS)
    throw exception("failed to create instance [%s]", vk_utils_result_string(result));
  return 0;
}


int me::Vulkan::get_physical_device_queue_family(const VkPhysicalDevice physical_device, QueueFamilyIndices &indices)
{
  /* get queue family properties from physical device */
  uint32_t queue_family_property_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);

  VkQueueFamilyProperties queue_family_properties[queue_family_property_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties);
  return 0;
}

int me::Vulkan::get_physical_device_features(const VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures &features)
{
  vkGetPhysicalDeviceFeatures(physical_device, &features);
  return 0;
}

int me::Vulkan::get_physical_device_extensions(const VkPhysicalDevice physical_device, vector<VkExtensionProperties> &extensions)
{
  /* get extension properties from physical device */
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

  extensions.resize(extension_count);
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());
  return 0;
}

int me::Vulkan::get_physical_device_properties(const VkPhysicalDevice physical_device, VkPhysicalDeviceProperties &properties)
{
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
  return 0;
}

int me::Vulkan::get_physical_devices(vector<VkPhysicalDevice> &physical_devices)
{
  /* get the count of physical devices */
  uint32_t physical_device_count;
  vkEnumeratePhysicalDevices(instance_info.instance, &physical_device_count, nullptr);

  /* populate physical device array */
  physical_devices.resize(physical_device_count);
  vkEnumeratePhysicalDevices(instance_info.instance, &physical_device_count, physical_devices.data());
  return 0;
}

int me::Vulkan::create_physical_device(PhysicalDeviceCreateInfo &physical_device_create_info, PhysicalDeviceInfo &physical_device_info)
{
  for (const PhysicalDeviceInfo &physical_device : physical_device_create_info.physical_device_infos)
  {
    logger->debug("found device[%s]", physical_device.properties.deviceName);


    /* check required extensions */
    bool has_required = true;
    for (const char* required : physical_device_create_info.required_extensions)
    {
      bool found = false;
      for (VkExtensionProperties extension : physical_device.extensions)
      {
	if (strcmp(required, extension.extensionName) == 0)
	{
	  found = true;
	  break;
	}
      }
      if (!found)
	has_required = false;
    }
    if (!has_required)
      continue;

    /* choose device */
    if (physical_device_info.device == VK_NULL_HANDLE || physical_device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      physical_device_info = physical_device;
  }
  return 0;
}


int me::Vulkan::get_surface_formats(const VkSurfaceKHR surface, vector<VkSurfaceFormatKHR> &formats)
{
  uint32_t surface_format_count;
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface, &surface_format_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats count [%s]", vk_utils_result_string(result));

  formats.resize(surface_format_count);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface, &surface_format_count, formats.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::get_surface_present_modes(const VkSurfaceKHR surface, vector<VkPresentModeKHR> &present_modes)
{
  uint32_t present_mode_count;
  VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface, &present_mode_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes count [%s]", vk_utils_result_string(result));

  present_modes.resize(present_mode_count);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface, &present_mode_count, present_modes.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::get_surface_capabilities(const VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR &capabilities)
{
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_info.device, surface, &capabilities);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface capabilities [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::create_surface(SurfaceCreateInfo &surface_create_info, SurfaceInfo &surface_info)
{
  /* find format */
  static const VkFormat color_format = VK_FORMAT_B8G8R8A8_UNORM;
  static const VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

  if (surface_create_info.formats.size() == 1 && surface_create_info.formats.at(0).format == VK_FORMAT_UNDEFINED)
    surface_info.format = { color_format, color_space };

  for (uint32_t i = 0; i < surface_create_info.formats.size(); i++)
  {
    if (surface_create_info.formats.at(i).format == color_format && surface_create_info.formats.at(i).colorSpace == color_space)
    {
      surface_info.format = surface_create_info.formats.at(i);
      break;
    }
  }

  if (surface_create_info.formats.size() > 0)
    surface_info.format = surface_create_info.formats.at(0);


  /* find present mode */
  surface_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;
  for (uint32_t i = 0; i < surface_create_info.present_modes.size(); i++)
  {
    if (surface_create_info.present_modes.at(i) == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      surface_info.present_mode = surface_create_info.present_modes.at(i);
      break;
    }
  }
 

  /* get extent */
  if (surface_info.capabilities.currentExtent.width != UINT32_MAX)
    surface_info.extent = surface_info.capabilities.currentExtent;

  /* get the size of the surface */
  me_surface.get_size(surface_info.extent.width, surface_info.extent.height);

  /* if extent.width/height is more than max image width/height; set the extent.width/height to max image width/height */
  surface_info.extent.width = std::min(surface_info.extent.width, surface_info.capabilities.maxImageExtent.width);
  surface_info.extent.height = std::min(surface_info.extent.height, surface_info.capabilities.maxImageExtent.height);

  /* if extent.width/height is less than min image width/height; set the extent.width/height to min image width/height */
  surface_info.extent.width = std::max(surface_info.extent.width, surface_info.capabilities.minImageExtent.width);
  surface_info.extent.width = std::max(surface_info.extent.height, surface_info.capabilities.minImageExtent.height);
  return 0;
}


int me::Vulkan::create_logical_device(LogicalDeviceCreateInfo &logical_device_create_info, LogicalDeviceInfo &logical_device_info)
{
  logical_device_create_info.info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  logical_device_create_info.info.pNext = nullptr;
  logical_device_create_info.info.flags = 0;
  logical_device_create_info.info.queueCreateInfoCount = (uint32_t) logical_device_create_info.device_queue_create_infos.size();
  logical_device_create_info.info.pQueueCreateInfos = logical_device_create_info.device_queue_create_infos.data();
  logical_device_create_info.info.enabledLayerCount = 0;
  logical_device_create_info.info.ppEnabledLayerNames = nullptr;
  logical_device_create_info.info.enabledExtensionCount = (uint32_t) physical_device_info.extensions.size();
  logical_device_create_info.info.ppEnabledExtensionNames = logical_device_create_info.extension_names.data();
  logical_device_create_info.info.pEnabledFeatures = &physical_device_info.features;
 
  VkResult result = vkCreateDevice(physical_device_info.device, &logical_device_create_info.info, nullptr, &logical_device_info.device);
  if (result != VK_SUCCESS)
    throw exception("failed to create device [%s]", vk_utils_result_string(result));
  return 0;

  return 0;
}


int me::Vulkan::create_command_pool(CommandPoolCreateInfo &command_pool_create_info, CommandPoolInfo &command_pool_info)
{
  command_pool_create_info.info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.info.pNext = nullptr;
  command_pool_create_info.info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.info.queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
 
  VkResult result = vkCreateCommandPool(command_pool_create_info.device, &command_pool_create_info.info, nullptr, &command_pool_info.command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));
  return 0;
}


int me::Vulkan::create_swapchain(SwapchainCreateInfo &swapchain_create_info, SwapchainInfo &swapchain_info)
{
  swapchain_create_info.info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.info.pNext = nullptr;
  swapchain_create_info.info.flags = 0;
  swapchain_create_info.info.surface = swapchain_create_info.surface;
  swapchain_create_info.info.minImageCount = swapchain_create_info.min_image_count;
  swapchain_create_info.info.imageFormat = swapchain_info.image_format;
  swapchain_create_info.info.imageColorSpace = swapchain_info.image_color_space;
  swapchain_create_info.info.imageExtent = swapchain_info.image_extent;
  swapchain_create_info.info.imageArrayLayers = 1;
  swapchain_create_info.info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  swapchain_create_info.info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.info.queueFamilyIndexCount = 0;
  swapchain_create_info.info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.info.preTransform = swapchain_create_info.pre_transform;
  swapchain_create_info.info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.info.presentMode = swapchain_create_info.present_mode;
  swapchain_create_info.info.clipped = VK_TRUE;
  swapchain_create_info.info.oldSwapchain = VK_NULL_HANDLE; /* TODO */
 
  VkResult result = vkCreateSwapchainKHR(swapchain_create_info.device, &swapchain_create_info.info, nullptr, &swapchain_info.swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", vk_utils_result_string(result));

  /* get swapchain images */
  uint32_t image_count;
  result = vkGetSwapchainImagesKHR(swapchain_create_info.device, swapchain_info.swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", vk_utils_result_string(result));

  swapchain_info.images.resize(image_count);
  result = vkGetSwapchainImagesKHR(swapchain_create_info.device, swapchain_info.swapchain, &image_count, swapchain_info.images.data());
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

    result = vkCreateImageView(swapchain_create_info.device, &image_view_create_info, nullptr, &swapchain_info.image_views.at(i));
    if (result != VK_SUCCESS)
      throw exception("failed to create image view [%s]", vk_utils_result_string(result));
  }
  return 0;
}


int me::Vulkan::create_pipeline_layout(PipelineLayoutCreateInfo &pipeline_layout_create_info, PipelineLayoutInfo &pipeline_layout_info)
{
  pipeline_layout_create_info.info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.info.pNext = nullptr;
  pipeline_layout_create_info.info.flags = 0;
  pipeline_layout_create_info.info.setLayoutCount = 0;
  pipeline_layout_create_info.info.pSetLayouts = nullptr;
  pipeline_layout_create_info.info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.info.pPushConstantRanges = nullptr;
 
  VkResult result = vkCreatePipelineLayout(pipeline_layout_create_info.device, &pipeline_layout_create_info.info, nullptr, &pipeline_layout_info.pipeline_layout);
  if (result != VK_SUCCESS)
    throw exception("failed to create pipeline layout [%s]", vk_utils_result_string(result));
  return 0;
}


int me::Vulkan::create_render_pass(RenderPassCreateInfo &render_pass_create_info, RenderPassInfo &render_pass_info)
{
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
  VkResult result = vkCreateShaderModule(logical_device_info.device, &shader_create_info, nullptr, &shader_module);
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
