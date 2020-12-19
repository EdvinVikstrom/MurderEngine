#include "Vulkan.hpp"

#include "VulkanHelper.hpp"

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
  required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  setup_instance();
  setup_physical_device();
  setup_logical_device();
  setup_command_pool();
  setup_surface();
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
  for (uint32_t i = 0; i < swapchain_info.image_views.count; i++)
    vkDestroyImageView(logical_device_info.device, swapchain_info.image_views[i], nullptr);

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
  uint32_t extension_count;
  const char** extensions = me_surface.get_extensions(extension_count);

  AppConfig app_config = engine->get_app_config();

  VkApplicationInfo application_info = { };
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pNext = nullptr;
  application_info.pApplicationName = app_config.name;
  application_info.applicationVersion = app_config.version;
  application_info.pEngineName = ME_ENGINE_NAME;
  application_info.engineVersion = VK_MAKE_VERSION(
	ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo instance_create_info = { };
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = 0;
  instance_create_info.pApplicationInfo = &application_info;
  instance_create_info.enabledLayerCount = 0;
  instance_create_info.ppEnabledLayerNames = nullptr;
  instance_create_info.enabledExtensionCount = extension_count;
  instance_create_info.ppEnabledExtensionNames = extensions;

  VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_info.instance);
  if (result != VK_SUCCESS)
    throw exception("failed to create instance [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_physical_device()
{
  /* get physical devices */
  uint32_t physical_device_count;
  get_physical_devices(instance_info.instance, physical_device_count, nullptr);
  VkPhysicalDevice physical_devices[physical_device_count];
  get_physical_devices(instance_info.instance, physical_device_count, physical_devices);

  /* get physical device infos */
  PhysicalDeviceInfo physical_device_infos[physical_device_count];
  get_physical_device_infos(physical_device_count, physical_devices, physical_device_infos);

  for (const PhysicalDeviceInfo &physical_device_info : physical_device_infos)
  {
    logger->debug("found device[%s]", physical_device_info.properties.deviceName);

    /* check required extensions */
    if (!has_required_extensions(physical_device_info, required_device_extensions))
      continue;

    /* check required queue family */
    if (!physical_device_info.queue_family_indices.graphics.has())
      continue;

    /* choose device */
    if (this->physical_device_info.device == VK_NULL_HANDLE || physical_device_info.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      this->physical_device_info = physical_device_info;
  }
  return 0;
}

int me::Vulkan::setup_logical_device()
{
  /* device queue create info(s) */
  uint32_t device_queue_count = 1;
  VkDeviceQueueCreateInfo device_queue_create_infos[device_queue_count];

  /* graphics queue */
  uint32_t graphics_queue_index = physical_device_info.queue_family_indices.graphics.value();
  float queue_priorities[1] = {1.0F};
  get_logical_device_queue_create_info(graphics_queue_index, 1, queue_priorities, device_queue_create_infos[0]);


  /* extension names */
  uint32_t extension_count = physical_device_info.extensions.count;
  const char* extension_names[extension_count];
  get_extension_names(extension_count, physical_device_info.extensions.ptr, extension_names);


  VkDeviceCreateInfo device_create_info = { };
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = device_queue_count;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.enabledLayerCount = 0;
  device_create_info.ppEnabledLayerNames = nullptr;
  device_create_info.enabledExtensionCount = extension_count;
  device_create_info.ppEnabledExtensionNames = extension_names;
  device_create_info.pEnabledFeatures = &physical_device_info.features;

  VkResult result = vkCreateDevice(physical_device_info.device, &device_create_info, nullptr, &logical_device_info.device);
  if (result != VK_SUCCESS)
    throw exception("failed to create device [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_command_pool()
{
  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
 
  VkResult result = vkCreateCommandPool(logical_device_info.device, &command_pool_create_info, nullptr, &command_pool_info.command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_surface()
{
  me_surface.create_surface(instance_info.instance, nullptr, &surface_info.surface);


  /* get surface formats */
  uint32_t format_count;
  get_surface_formats(physical_device_info.device, surface_info.surface, format_count, nullptr);
  VkSurfaceFormatKHR formats[format_count];
  get_surface_formats(physical_device_info.device, surface_info.surface, format_count, formats);

  /* get present modes */
  uint32_t present_mode_count;
  get_surface_present_modes(physical_device_info.device, surface_info.surface, present_mode_count, nullptr);
  VkPresentModeKHR present_modes[present_mode_count];
  get_surface_present_modes(physical_device_info.device, surface_info.surface, present_mode_count, present_modes);

  get_surface_capabilities(physical_device_info.device, surface_info.surface, surface_info.capabilities);

  /* get surface format */
  find_surface_format(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, format_count, formats, surface_info.format);

  /* get present mode */
  surface_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;
  if (has_present_mode(present_mode_count, present_modes, VK_PRESENT_MODE_MAILBOX_KHR))
    surface_info.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;

  /* get extent */
  if (surface_info.capabilities.currentExtent.width != UINT32_MAX)
    surface_info.extent = surface_info.capabilities.currentExtent;

  /* get the size of the surface */
  me_surface.get_size(surface_info.extent.width, surface_info.extent.height);
  get_surface_extent(surface_info.capabilities.maxImageExtent, surface_info.capabilities.minImageExtent, surface_info.extent);
  return 0;
}

int me::Vulkan::setup_swapchain()
{
  swapchain_info.image_format = surface_info.format.format;
  swapchain_info.image_color_space = surface_info.format.colorSpace;
  swapchain_info.image_extent = surface_info.extent;

  uint32_t min_image_count = math::min(surface_info.capabilities.minImageCount + 1,
      surface_info.capabilities.maxImageCount > 0 ? surface_info.capabilities.maxImageCount : -1);
  VkSurfaceTransformFlagBitsKHR pre_transform = surface_info.capabilities.currentTransform;
  VkPresentModeKHR present_mode = surface_info.present_mode;

  VkSwapchainCreateInfoKHR swapchain_create_info = { };
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = nullptr;
  swapchain_create_info.flags = 0;
  swapchain_create_info.surface = surface_info.surface;
  swapchain_create_info.minImageCount = min_image_count;
  swapchain_create_info.imageFormat = swapchain_info.image_format;
  swapchain_create_info.imageColorSpace = swapchain_info.image_color_space;
  swapchain_create_info.imageExtent = swapchain_info.image_extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (surface_info.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.preTransform = pre_transform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = present_mode;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; /* TODO */
 
  VkResult result = vkCreateSwapchainKHR(logical_device_info.device, &swapchain_create_info, nullptr, &swapchain_info.swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", vk_utils_result_string(result));

  /* get swapchain images */
  result = vkGetSwapchainImagesKHR(logical_device_info.device, swapchain_info.swapchain, &swapchain_info.images.count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", vk_utils_result_string(result));

  alloc.allocate_array(swapchain_info.images);
  result = vkGetSwapchainImagesKHR(logical_device_info.device, swapchain_info.swapchain, &swapchain_info.images.count, swapchain_info.images.ptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images [%s]", vk_utils_result_string(result));


  /* create swapchain image views */
  alloc.allocate_array(swapchain_info.images.count, swapchain_info.image_views);
  for (uint32_t i = 0; i < swapchain_info.image_views.count; i++)
  {
    VkImageViewCreateInfo image_view_create_info = { };
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = swapchain_info.images[i];
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

    result = vkCreateImageView(logical_device_info.device, &image_view_create_info, nullptr, &swapchain_info.image_views[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create image view [%s]", vk_utils_result_string(result));
  }
  return 0;
}

int me::Vulkan::setup_pipeline_layout()
{
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = { };
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.pNext = nullptr;
  pipeline_layout_create_info.flags = 0;
  pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_create_info.pSetLayouts = nullptr;
  pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.pPushConstantRanges = nullptr;
 
  VkResult result = vkCreatePipelineLayout(logical_device_info.device, &pipeline_layout_create_info, nullptr, &pipeline_layout_info.pipeline_layout);
  if (result != VK_SUCCESS)
    throw exception("failed to create pipeline layout [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_render_pass()
{
  return 0;
}


int me::Vulkan::setup_viewports()
{
  alloc.allocate_array(1, viewport_info.viewports);
  viewport_info.viewports[0] = {
      .x = 0.0F, .y = 0.0F,
      .width = (float) swapchain_info.image_extent.width, .height = (float) swapchain_info.image_extent.height,
      .minDepth = 0.0F, .maxDepth = 1.0F};

  alloc.allocate_array(1, viewport_info.scissors);
  viewport_info.scissors[0] = {
      .offset = {0, 0},
      .extent = swapchain_info.image_extent};

  viewport_info.pipline_viewport_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.pipline_viewport_stage_create_info.pNext = nullptr;
  viewport_info.pipline_viewport_stage_create_info.flags = 0;
  viewport_info.pipline_viewport_stage_create_info.viewportCount = viewport_info.viewports.count;
  viewport_info.pipline_viewport_stage_create_info.pViewports = viewport_info.viewports.ptr;
  viewport_info.pipline_viewport_stage_create_info.scissorCount = viewport_info.scissors.count;
  viewport_info.pipline_viewport_stage_create_info.pScissors = viewport_info.scissors.ptr;
  return 0;
}

int me::Vulkan::setup_rasterizer()
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

int me::Vulkan::setup_multisampling()
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

int me::Vulkan::setup_color_blend()
{
  alloc.allocate_array(1, color_blend_info.pipeline_color_blend_attachment_states);
  color_blend_info.pipeline_color_blend_attachment_states[0] = {
      VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  color_blend_info.pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.pipeline_color_blend_state_create_info.pNext = nullptr;
  color_blend_info.pipeline_color_blend_state_create_info.flags = 0;
  color_blend_info.pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_info.pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_info.pipeline_color_blend_state_create_info.attachmentCount = color_blend_info.pipeline_color_blend_attachment_states.count;
  color_blend_info.pipeline_color_blend_state_create_info.pAttachments = color_blend_info.pipeline_color_blend_attachment_states.ptr;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[0] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[1] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[2] = 0.0F;
  color_blend_info.pipeline_color_blend_state_create_info.blendConstants[3] = 0.0F;
  return 0;
}

int me::Vulkan::setup_dynamic()
{
  alloc.allocate_array(2, dynamic_info.dynamics);
  dynamic_info.dynamics[0] = VK_DYNAMIC_STATE_VIEWPORT;
  dynamic_info.dynamics[1] = VK_DYNAMIC_STATE_LINE_WIDTH;

  dynamic_info.pipline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_info.pipline_dynamic_state_create_info.pNext = nullptr;
  dynamic_info.pipline_dynamic_state_create_info.flags = 0;
  dynamic_info.pipline_dynamic_state_create_info.dynamicStateCount = dynamic_info.dynamics.count;
  dynamic_info.pipline_dynamic_state_create_info.pDynamicStates = dynamic_info.dynamics.ptr;
  return 0;
}


int me::Vulkan::get_physical_device_infos(uint32_t &physical_device_count, VkPhysicalDevice* physical_devices, PhysicalDeviceInfo* physical_device_infos)
{
  for (uint32_t i = 0; i < physical_device_count; i++)
  {
    PhysicalDeviceInfo &physical_device_info = physical_device_infos[i];
    physical_device_info.device = physical_devices[i];
    get_physical_device_properties(physical_devices[i], physical_device_info.properties);
    get_physical_device_features(physical_devices[i], physical_device_info.features);
    get_physical_device_queue_family(physical_devices[i], physical_device_info.queue_family_indices);

    /* get physical device extensions */
    get_physical_device_extensions(physical_devices[i], physical_device_info.extensions.count, nullptr);
    alloc.allocate_array(physical_device_info.extensions);
    get_physical_device_extensions(physical_devices[i], physical_device_info.extensions.count, physical_device_info.extensions.ptr);
  }
  return 0;
}


int me::Vulkan::get_physical_device_queue_family(const VkPhysicalDevice physical_device, QueueFamilyIndices &indices)
{
  /* get queue family properties from physical device */
  uint32_t queue_family_property_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);

  VkQueueFamilyProperties queue_family_properties[queue_family_property_count];
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties);

  for (uint32_t i = 0; i < queue_family_property_count; i++)
  {
    if (queue_family_properties[i].queueCount > 0)
    {
      if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	indices.graphics.assign(i);
    }
  }
  return 0;
}

int me::Vulkan::get_physical_device_features(const VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures &features)
{
  vkGetPhysicalDeviceFeatures(physical_device, &features);
  return 0;
}

int me::Vulkan::get_physical_device_extensions(const VkPhysicalDevice physical_device, uint32_t &extension_count, VkExtensionProperties* extensions)
{
  /* get extension count of physical device */
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
  
  if (extensions == nullptr)
    return 0;

  /* pipulate extension array */
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions);
  return 0;
}

int me::Vulkan::get_physical_device_properties(const VkPhysicalDevice physical_device, VkPhysicalDeviceProperties &properties)
{
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  return 0;
}

int me::Vulkan::get_physical_devices(const VkInstance instance, uint32_t &physical_device_count, VkPhysicalDevice* physical_devices)
{
  /* get the count of physical devices */
  vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);

  if (physical_devices == nullptr)
    return 0;

  /* populate physical device array */
  vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);
  return 0;
}


int me::Vulkan::get_surface_formats(const VkPhysicalDevice physical_device, const VkSurfaceKHR surface, uint32_t &format_count, VkSurfaceFormatKHR* formats)
{
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats count [%s]", vk_utils_result_string(result));

  if (formats == nullptr)
    return 0;

  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface formats [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::get_surface_present_modes(const VkPhysicalDevice physical_device, const VkSurfaceKHR surface, uint32_t &present_mode_count, VkPresentModeKHR* present_modes)
{
  VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes count [%s]", vk_utils_result_string(result));

  if (present_modes == nullptr)
    return 0;

  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface present modes [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::get_surface_capabilities(const VkPhysicalDevice physical_device, const VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR &capabilities)
{
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
  if (result != VK_SUCCESS)
    throw exception("failed to get physical device surface capabilities [%s]", vk_utils_result_string(result));
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
