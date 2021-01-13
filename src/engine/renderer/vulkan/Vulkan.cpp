#include "Vulkan.hpp"

#include "VulkanHelper.hpp"

#include <lme/map.hpp>
#include <lme/math/math.hpp>
#include <lme/string.hpp>
#include <lme/set.hpp>
#include <vulkan/vulkan_core.h>

me::Vulkan::Vulkan(Surface* me_surface)
  : Renderer("vulkan"), me_surface(me_surface), logger("Vulkan")
{
}

int me::Vulkan::initialize(const ModuleInfo module_info)
{
  //logger.trace_all();

  setup_extensions();
  setup_layers();
  setup_instance(module_info.engine_info);
#ifndef NDEBUG
  setup_debug_messenger();
  setup_debug_report();
#endif
  setup_me_surface();
  setup_device_extensions();
  setup_device_layers();
  setup_physical_device();
  setup_logical_device();
  setup_surface();
  setup_swapchain();
  setup_image_views();
  setup_render_pass();
  setup_shaders();
  setup_pipeline_layout();
  setup_graphics_pipeline();
  setup_framebuffers();
  setup_command_pool();
  setup_command_buffers();
  setup_synchronization();

  render_info.state = RenderInfo::ACTIVE_STATE;

  return 0;
}

int me::Vulkan::terminate(const ModuleInfo module_info)
{
  vkDeviceWaitIdle(logical_device_info.device);

#ifndef NDEBUG
  terminate_debug_messenger();
  terminate_debug_report();
#endif

  cleanup_synchronization();
  cleanup_command_buffers();
  cleanup_command_pool();
  cleanup_framebuffers();
  cleanup_graphics_pipeline();
  cleanup_pipeline_layout();
  cleanup_shaders();
  cleanup_render_pass();
  cleanup_image_views();
  cleanup_swapchain();
  cleanup_surface();
  cleanup_logical_device();
  cleanup_instance();

  return 0;
}

int me::Vulkan::tick(const ModuleInfo module_info)
{
  if (render_info.state == RenderInfo::ACTIVE_STATE)
    render(render_info);
  else if (render_info.state == RenderInfo::NO_SWAPCHAIN_STATE)
    refresh_swapchains();
  return 0;
}


int me::Vulkan::render(RenderInfo &render_info)
{
  vkWaitForFences(logical_device_info.device, 1, &synchronization_info.in_flight[render_info.frame_index], VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(logical_device_info.device, swapchain_info.swapchain, UINT64_MAX,
      synchronization_info.image_available[render_info.frame_index], VK_NULL_HANDLE, &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    refresh_swapchains();
    return 0;
  }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw exception("failed to acquire next image [%s]", vk_utils_result_string(result));


  if (synchronization_info.images_in_flight[image_index] != VK_NULL_HANDLE)
    vkWaitForFences(logical_device_info.device, 1, &synchronization_info.images_in_flight[image_index], VK_TRUE, UINT64_MAX);
  synchronization_info.images_in_flight[image_index] = synchronization_info.in_flight[render_info.frame_index];


  /* create wait semaphores */
  static uint32_t wait_semaphore_count = 1;
  VkSemaphore wait_semaphores[wait_semaphore_count];
  wait_semaphores[0] = synchronization_info.image_available[render_info.frame_index];

  /* create wait stages */
  static uint32_t wait_stage_count = 1;
  VkPipelineStageFlags wait_stages[wait_stage_count];
  wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  /* get command buffers */
  static uint32_t command_buffer_count = 1;
  VkCommandBuffer command_buffers[command_buffer_count];
  command_buffers[0] = this->command_buffers[image_index];

  /* create signal semaphores */
  static uint32_t signal_semaphore_count = 1;
  VkSemaphore signal_semaphores[signal_semaphore_count];
  signal_semaphores[0] = synchronization_info.render_finished[render_info.frame_index];

  /* create submit infos */
  static uint32_t submit_info_count = 1;
  VkSubmitInfo submit_infos[submit_info_count];
  submit_infos[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_infos[0].pNext = nullptr;
  submit_infos[0].waitSemaphoreCount = wait_semaphore_count;
  submit_infos[0].pWaitSemaphores = wait_semaphores;
  submit_infos[0].pWaitDstStageMask = wait_stages;
  submit_infos[0].commandBufferCount = command_buffer_count;
  submit_infos[0].pCommandBuffers = command_buffers;
  submit_infos[0].signalSemaphoreCount = signal_semaphore_count;
  submit_infos[0].pSignalSemaphores = signal_semaphores;

  vkResetFences(logical_device_info.device, 1, &synchronization_info.in_flight[render_info.frame_index]);

  result = vkQueueSubmit(queue_info.graphics_queue, submit_info_count, submit_infos, synchronization_info.in_flight[render_info.frame_index]);
  if (result != VK_SUCCESS)
    throw exception("failed to queue submit [%s]", vk_utils_result_string(result));


  /* get swapchains */
  static uint32_t swapchain_count = 1;
  VkSwapchainKHR swapchains[swapchain_count];
  swapchains[0] = swapchain_info.swapchain;

  /* create present info */
  VkPresentInfoKHR present_info = { };
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.waitSemaphoreCount = signal_semaphore_count;
  present_info.pWaitSemaphores = signal_semaphores;
  present_info.swapchainCount = swapchain_count;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;

  result = vkQueuePresentKHR(queue_info.present_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      render_info.flags & RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT)
  {
    render_info.flags &= ~RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
    refresh_swapchains();
  }else if (result != VK_SUCCESS)
    throw exception("failed to queue present [%s]", vk_utils_result_string(result));


  /* increment the frame index and reset to 0 if equals MAX_FRAMES_IN_FLIGHT */
  render_info.frame_index++;
  if (render_info.frame_index == MAX_FRAMES_IN_FLIGHT)
    render_info.frame_index = 0;
  return 0;
}


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

int me::Vulkan::setup_me_surface()
{
  me_surface->vk_create_surface(instance_info.instance, nullptr, &surface_info.surface);
  me_surface->register_resize_surface_callback(callback_surface_resize, this);
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
  get_physical_device_infos(physical_device_count, physical_devices, physical_device_infos);

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
    get_logical_device_queue_create_info(unique_queue_families.at(i), 1, queue_priorities, device_queue_create_infos[i]);

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

int me::Vulkan::setup_surface()
{
  logger.debug("> SETUP_SURFACE");
  return 0;
}

int me::Vulkan::setup_swapchain()
{
  logger.debug("> SETUP_SWAPCHAIN");

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_info.device, surface_info.surface, &swapchain_info.surface_capabilities);

  /* get surface formats */
  uint32_t surface_format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface_info.surface, &surface_format_count, nullptr);
  VkSurfaceFormatKHR surface_formats[surface_format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface_info.surface, &surface_format_count, surface_formats);

  /* get present modes */
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface_info.surface, &present_mode_count, nullptr);
  VkPresentModeKHR present_modes[present_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface_info.surface, &present_mode_count, present_modes);
  swapchain_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;

  find_surface_format(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, {surface_format_count, surface_formats}, swapchain_info.surface_format);
  find_present_mode(VK_PRESENT_MODE_MAILBOX_KHR, {present_mode_count, present_modes}, swapchain_info.present_mode);
  find_image_extent(swapchain_info.surface_capabilities.currentExtent, swapchain_info.image_extent);
  me_surface->get_framebuffer_size(swapchain_info.image_extent.width, swapchain_info.image_extent.height);
  get_extent(swapchain_info.surface_capabilities.maxImageExtent, swapchain_info.surface_capabilities.minImageExtent, swapchain_info.image_extent);


  swapchain_info.image_format = swapchain_info.surface_format.format;
  swapchain_info.image_color_space = swapchain_info.surface_format.colorSpace;
  swapchain_info.image_extent = swapchain_info.image_extent;

  uint32_t min_image_count = math::min(swapchain_info.surface_capabilities.minImageCount + 1,
      swapchain_info.surface_capabilities.maxImageCount > 0 ? swapchain_info.surface_capabilities.maxImageCount : -1);
  VkSurfaceTransformFlagBitsKHR pre_transform = swapchain_info.surface_capabilities.currentTransform;
  VkPresentModeKHR present_mode = swapchain_info.present_mode;

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
    (swapchain_info.surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (swapchain_info.surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
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
 return 0;
}

int me::Vulkan::setup_image_views()
{
  logger.debug("> SETUP_IMAGE_VIEWS");

  alloc.allocate_array(swapchain_info.images.count, swapchain_info.image_views);
  for (uint32_t i = 0; i < swapchain_info.image_views.count; i++)
  {
    VkImageViewCreateInfo image_view_create_info = { };
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = swapchain_info.images[i];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = swapchain_info.surface_format.format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(logical_device_info.device, &image_view_create_info, nullptr, &swapchain_info.image_views[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create image view [%s]", vk_utils_result_string(result));
  }
  return 0;
}

int me::Vulkan::setup_pipeline_layout()
{
  logger.debug("> SETUP_PIPELINE_LAYOUT");

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
  logger.debug("> SETUP_RENDER_PASS");

  /* create subpass dependencies */
  uint32_t subpass_dependency_count = 1;
  VkSubpassDependency subpass_dependencies[subpass_dependency_count];
  subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependencies[0].dstSubpass = 0;
  subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependencies[0].srcAccessMask = 0;
  subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subpass_dependencies[0].dependencyFlags = 0;

  uint32_t color_attachment_description_count = 1;
  VkAttachmentDescription color_attachment_descriptions[color_attachment_description_count];
  color_attachment_descriptions[0].flags = 0;
  color_attachment_descriptions[0].format = swapchain_info.image_format;
  color_attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  uint32_t color_attachment_reference_count = 1;
  VkAttachmentReference color_attachment_references[color_attachment_reference_count];
  color_attachment_references[0].attachment = 0;
  color_attachment_references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  uint32_t subpass_description_count = 1;
  VkSubpassDescription subpass_descriptions[subpass_description_count];
  subpass_descriptions[0].flags = 0;
  subpass_descriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_descriptions[0].inputAttachmentCount = 0;
  subpass_descriptions[0].pInputAttachments = nullptr;
  subpass_descriptions[0].colorAttachmentCount = color_attachment_reference_count;
  subpass_descriptions[0].pColorAttachments = color_attachment_references;
  subpass_descriptions[0].pResolveAttachments = nullptr;
  subpass_descriptions[0].pDepthStencilAttachment = nullptr;
  subpass_descriptions[0].preserveAttachmentCount = 0;
  subpass_descriptions[0].pPreserveAttachments = nullptr;

  VkRenderPassCreateInfo render_pass_create_info = { };
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.pNext = nullptr;
  render_pass_create_info.flags = 0;
  render_pass_create_info.attachmentCount = color_attachment_description_count;
  render_pass_create_info.pAttachments = color_attachment_descriptions;
  render_pass_create_info.subpassCount = subpass_description_count;
  render_pass_create_info.pSubpasses = subpass_descriptions;
  render_pass_create_info.dependencyCount = subpass_dependency_count;
  render_pass_create_info.pDependencies = subpass_dependencies;

  VkResult result = vkCreateRenderPass(logical_device_info.device, &render_pass_create_info, nullptr, &render_pass_info.render_pass);
  if (result != VK_SUCCESS)
    throw exception("failed to create render pass [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_shaders()
{
  logger.debug("> SETUP_SHADERS");

  for (const Shader* shader : shader_info.shaders)
    create_shader_module(shader);

  shader_info.vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  shader_info.vertex_input_state.pNext = nullptr;
  shader_info.vertex_input_state.flags = 0;
  shader_info.vertex_input_state.vertexBindingDescriptionCount = 0;
  shader_info.vertex_input_state.pVertexBindingDescriptions = nullptr;
  shader_info.vertex_input_state.vertexAttributeDescriptionCount = 0;
  shader_info.vertex_input_state.pVertexAttributeDescriptions = nullptr;

  shader_info.input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  shader_info.input_assembly_state.pNext = nullptr;
  shader_info.input_assembly_state.flags = 0;
  shader_info.input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  shader_info.input_assembly_state.primitiveRestartEnable = VK_FALSE;
  return 0;
}

int me::Vulkan::setup_graphics_pipeline()
{
  logger.debug("> SETUP_GRAPHICS_PIPELINE");

  setup_viewports();
  setup_rasterizer();
  setup_multisampling();
  setup_color_blend();
  setup_dynamic();

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = { };
  graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_create_info.pNext = nullptr;
  graphics_pipeline_create_info.flags = 0;
  graphics_pipeline_create_info.stageCount = static_cast<uint32_t>(shader_info.pipeline_shader_stage_create_infos.size());
  graphics_pipeline_create_info.pStages = shader_info.pipeline_shader_stage_create_infos.data();
  graphics_pipeline_create_info.pVertexInputState = &shader_info.vertex_input_state;
  graphics_pipeline_create_info.pInputAssemblyState = &shader_info.input_assembly_state;
  graphics_pipeline_create_info.pViewportState = &viewport_info.pipline_viewport_stage_create_info;
  graphics_pipeline_create_info.pRasterizationState = &rasterizer_info.pipeline_rasterization_state_create_info;
  graphics_pipeline_create_info.pMultisampleState = &multisampling_info.pipeline_multisample_state_create_info;
  graphics_pipeline_create_info.pDepthStencilState = nullptr;
  graphics_pipeline_create_info.pColorBlendState = &color_blend_info.pipeline_color_blend_state_create_info;
  graphics_pipeline_create_info.pDynamicState = nullptr;
  graphics_pipeline_create_info.layout = pipeline_layout_info.pipeline_layout;
  graphics_pipeline_create_info.renderPass = render_pass_info.render_pass;
  graphics_pipeline_create_info.subpass = 0;
  graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  graphics_pipeline_create_info.basePipelineIndex = -1;

  VkResult result = vkCreateGraphicsPipelines(logical_device_info.device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &graphics_pipeline_info.pipeline);
  if (result != VK_SUCCESS)
    throw exception("failed to create graphics piplines [%s]", vk_utils_result_string(result));

  for (const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info : shader_info.pipeline_shader_stage_create_infos)
    vkDestroyShaderModule(logical_device_info.device, pipeline_shader_stage_create_info.module, nullptr);
  shader_info.pipeline_shader_stage_create_infos.clear();
  return 0;
}

int me::Vulkan::setup_framebuffers()
{
  logger.debug("> SETUP_FRAMEBUFFERS");

  alloc.allocate_array(swapchain_info.image_views.count, framebuffer_info.framebuffers);

  for (uint32_t i = 0; i < swapchain_info.image_views.count; i++)
  {
    uint32_t attachment_count = 1;
    VkImageView attachments[attachment_count];
    attachments[0] = swapchain_info.image_views[i];

    VkFramebufferCreateInfo framebuffer_create_info = { };
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.pNext = nullptr;
    framebuffer_create_info.flags = 0;
    framebuffer_create_info.renderPass = render_pass_info.render_pass;
    framebuffer_create_info.attachmentCount = attachment_count;
    framebuffer_create_info.pAttachments = attachments;
    framebuffer_create_info.width = swapchain_info.image_extent.width;
    framebuffer_create_info.height = swapchain_info.image_extent.height;
    framebuffer_create_info.layers = 1;

    VkResult result = vkCreateFramebuffer(logical_device_info.device, &framebuffer_create_info, nullptr, &framebuffer_info.framebuffers[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create framebuffer [%s]", vk_utils_result_string(result));
  }
  return 0;
}

int me::Vulkan::setup_command_pool()
{
  logger.debug("> SETUP_COMMAND_POOL");

  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = 0;
  command_pool_create_info.queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
 
  VkResult result = vkCreateCommandPool(logical_device_info.device, &command_pool_create_info, nullptr, &command_pool_info.command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_command_buffers()
{
  logger.debug("> SETUP_COMMAND_BUFFERS");

  alloc.allocate_array(framebuffer_info.framebuffers.count, command_buffers);

  VkCommandBufferAllocateInfo command_buffer_allocate_info = { };
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.pNext = nullptr;
  command_buffer_allocate_info.commandPool = command_pool_info.command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = command_buffers.count;

  VkResult result = vkAllocateCommandBuffers(logical_device_info.device, &command_buffer_allocate_info, command_buffers.ptr);
  if (result != VK_SUCCESS)
    throw exception("failed to allocate command buffers [%s]", vk_utils_result_string(result));

  for (uint32_t i = 0; i < command_buffers.count; i++)
  {
    /* create command buffer begin info */
    VkCommandBufferBeginInfo command_buffer_begin_info = { };
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    /* begin command buffer */
    result = vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info);
    if (result != VK_SUCCESS)
      throw exception("failed to begin command buffer [%s]", vk_utils_result_string(result));

    start_render_pass(command_buffers[i], framebuffer_info.framebuffers[i]);

    /* end command buffer */
    result = vkEndCommandBuffer(command_buffers[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to end command buffer [%s]", result);
  }
  return 0;
}

int me::Vulkan::setup_synchronization()
{
  logger.debug("> SETUP_SEMAPHORES");

  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.image_available);
  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.render_finished);
  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.in_flight);
  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.images_in_flight);

  VkSemaphoreCreateInfo semaphore_create_info = { };
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = { };
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    /* create 'image available' semaphore */
    VkResult result = vkCreateSemaphore(logical_device_info.device, &semaphore_create_info, nullptr, &synchronization_info.image_available[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", vk_utils_result_string(result));

    /* create 'render finished' semaphore */
    result = vkCreateSemaphore(logical_device_info.device, &semaphore_create_info, nullptr, &synchronization_info.render_finished[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", vk_utils_result_string(result));

    /* create 'in flight' fence */
    result = vkCreateFence(logical_device_info.device, &fence_create_info, nullptr, &synchronization_info.in_flight[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create fence [%s]", vk_utils_result_string(result));

    synchronization_info.images_in_flight[i] = VK_NULL_HANDLE;
  }
  return 0;
}


int me::Vulkan::refresh_swapchains()
{
  uint32_t width, height;
  me_surface->get_framebuffer_size(width, height);
  if (width == 0 && height == 0)
  {
    render_info.state = RenderInfo::NO_SWAPCHAIN_STATE;
    return 1;
  }

  vkDeviceWaitIdle(logical_device_info.device);

  cleanup_framebuffers();
  cleanup_graphics_pipeline();
  cleanup_pipeline_layout();
  cleanup_render_pass();
  cleanup_image_views();
  cleanup_swapchain();

  setup_swapchain();
  setup_image_views();
  setup_render_pass();
  setup_shaders();
  setup_pipeline_layout();
  setup_graphics_pipeline();
  setup_framebuffers();
  setup_command_buffers();
  return 0;
}


int me::Vulkan::start_render_pass(VkCommandBuffer command_buffer, VkFramebuffer framebuffer)
{
  /* create render pass begin info */
  VkRenderPassBeginInfo render_pass_begin_info = { };
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.pNext = nullptr;
  render_pass_begin_info.renderPass = render_pass_info.render_pass;
  render_pass_begin_info.framebuffer = framebuffer;
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = swapchain_info.image_extent;

  /* create clear values */
  uint32_t clear_value_count = 1;
  VkClearValue clear_values[clear_value_count];
  clear_values[0] = { 0.0F, 0.0F, 0.0F, 1.0F };

  render_pass_begin_info.clearValueCount = clear_value_count;
  render_pass_begin_info.pClearValues = clear_values;

  /* begin render pass */
  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_info.pipeline);
  vkCmdDraw(command_buffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer);
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
  rasterizer_info.pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
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


int me::Vulkan::cleanup_instance()
{
  vkDestroyInstance(instance_info.instance, nullptr);
  return 0;
}

int me::Vulkan::cleanup_logical_device()
{
  vkDestroyDevice(logical_device_info.device, nullptr);
  return 0;
}

int me::Vulkan::cleanup_surface()
{
  vkDestroySurfaceKHR(instance_info.instance, surface_info.surface, nullptr);
  return 0;
}

int me::Vulkan::cleanup_swapchain()
{
  vkDestroySwapchainKHR(logical_device_info.device, swapchain_info.swapchain, nullptr);
  return 0;
}

int me::Vulkan::cleanup_image_views()
{
  for (uint32_t i = 0; i < swapchain_info.image_views.count; i++)
    vkDestroyImageView(logical_device_info.device, swapchain_info.image_views[i], nullptr);
  return 0;
}

int me::Vulkan::cleanup_pipeline_layout()
{
  vkDestroyPipelineLayout(logical_device_info.device, pipeline_layout_info.pipeline_layout, nullptr);
  return 0;
}

int me::Vulkan::cleanup_render_pass()
{
  vkDestroyRenderPass(logical_device_info.device, render_pass_info.render_pass, nullptr);
  return 0;
}

int me::Vulkan::cleanup_shaders()
{
  shader_info.pipeline_shader_stage_create_infos.clear();
  return 0;
}

int me::Vulkan::cleanup_graphics_pipeline()
{
  vkDestroyPipeline(logical_device_info.device, graphics_pipeline_info.pipeline, nullptr);
  return 0;
}

int me::Vulkan::cleanup_framebuffers()
{
  for (uint32_t i = 0; i < framebuffer_info.framebuffers.count; i++)
    vkDestroyFramebuffer(logical_device_info.device, framebuffer_info.framebuffers[i], nullptr);
  return 0;
}

int me::Vulkan::cleanup_command_pool()
{
  vkDestroyCommandPool(logical_device_info.device, command_pool_info.command_pool, nullptr);
  return 0;
}

int me::Vulkan::cleanup_command_buffers()
{
  vkFreeCommandBuffers(logical_device_info.device, command_pool_info.command_pool, command_buffers.count, command_buffers.ptr);
  return 0;
}

int me::Vulkan::cleanup_synchronization()
{
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroySemaphore(logical_device_info.device, synchronization_info.image_available[i], nullptr);
    vkDestroySemaphore(logical_device_info.device, synchronization_info.render_finished[i], nullptr);
    vkDestroyFence(logical_device_info.device, synchronization_info.in_flight[i], nullptr);
  }
  return 0;
}


int me::Vulkan::get_physical_device_infos(uint32_t &physical_device_count, VkPhysicalDevice* physical_devices, PhysicalDeviceInfo* physical_device_infos)
{
  for (uint32_t i = 0; i < physical_device_count; i++)
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
	physical_device_info.queue_family_properties.count, physical_device_info.queue_family_properties.ptr, physical_device_info.queue_family_indices);
  }
  return 0;
}


int me::Vulkan::create_shader_module(const Shader* shader)
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

  VkShaderStageFlagBits shader_stage_flag_bits;
  get_shader_stage_flag(shader->get_type(), shader_stage_flag_bits);

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = { };
  pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  pipeline_shader_stage_create_info.pNext = nullptr;
  pipeline_shader_stage_create_info.flags = 0;
  pipeline_shader_stage_create_info.stage = shader_stage_flag_bits;
  pipeline_shader_stage_create_info.module = shader_module;
  pipeline_shader_stage_create_info.pName = shader->get_config().entry_point;
  pipeline_shader_stage_create_info.pSpecializationInfo = nullptr;

  shader_info.pipeline_shader_stage_create_infos.push_back(pipeline_shader_stage_create_info);
  return 0;
}

int me::Vulkan::register_shader(Shader* shader)
{
  shader_info.shaders.push_back(shader);
  return 0;
}


int me::Vulkan::callback_surface_resize(uint32_t width, uint32_t height, void* ptr)
{
  Vulkan* instance = reinterpret_cast<Vulkan*>(ptr);
  instance->render_info.flags |= RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
  return 0;
}


#ifndef NDEBUG
int me::Vulkan::setup_debug_messenger()
{
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkCreateDebugUtilsMessengerEXT");

  VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = { };
  debug_utils_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_utils_messenger_create_info.pNext = nullptr;
  debug_utils_messenger_create_info.flags = 0;
  debug_utils_messenger_create_info.messageSeverity =
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  debug_utils_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  debug_utils_messenger_create_info.pfnUserCallback = debug_callback;
  debug_utils_messenger_create_info.pUserData = &logger;

  VkResult result = vkCreateDebugUtilsMessengerEXT(instance_info.instance, &debug_utils_messenger_create_info,
      nullptr, &debug_info.debug_utils_messenger);

  if (result != VK_SUCCESS)
    throw exception("failed to create debug utils messenger [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_debug_report()
{
  PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkCreateDebugReportCallbackEXT");

  VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info = { };
  debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debug_report_callback_create_info.pNext = nullptr;
  debug_report_callback_create_info.flags =
    VK_DEBUG_REPORT_ERROR_BIT_EXT |
    VK_DEBUG_REPORT_WARNING_BIT_EXT |
    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
    VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
    VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  debug_report_callback_create_info.pfnCallback = debug_callback;
  debug_report_callback_create_info.pUserData = &logger;

  VkResult result = vkCreateDebugReportCallbackEXT(instance_info.instance, &debug_report_callback_create_info,
      nullptr, &debug_info.debug_report_callback);
  if (result != VK_SUCCESS)
    throw exception("failed to create debug report callback [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::terminate_debug_messenger()
{
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkDestroyDebugUtilsMessengerEXT");

  vkDestroyDebugUtilsMessengerEXT(instance_info.instance, debug_info.debug_utils_messenger, nullptr);
  return 0;
}

int me::Vulkan::terminate_debug_report()
{
  PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkDestroyDebugReportCallbackEXT");

  vkDestroyDebugReportCallbackEXT(instance_info.instance, debug_info.debug_report_callback, nullptr);
  return 0;
}

VkBool32 me::Vulkan::debug_callback(VkDebugReportFlagsEXT debug_report_flags, VkDebugReportObjectTypeEXT debug_report_object_type,
    uint64_t object, size_t location, int32_t message_code,
    const char* layer_prefix, const char* message, void* user_data)
{
  Logger* logger = (Logger*) user_data;
  if (debug_report_flags == VK_DEBUG_REPORT_ERROR_BIT_EXT)
    logger->err("%s", message);
  else if (debug_report_flags == VK_DEBUG_REPORT_WARNING_BIT_EXT)
    logger->warn("%s", message);
  else if (debug_report_flags == VK_DEBUG_REPORT_WARNING_BIT_EXT)
    logger->warn("%s", message);
  else if (debug_report_flags == VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    logger->info("%s", message);
  else if (debug_report_flags == VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    logger->debug("%s", message);
  return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL me::Vulkan::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
  Logger* logger = (Logger*) user_data;
  if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    logger->err("[Validation Layer] %s", callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    logger->warn("[Validation Layer] %s", callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    logger->info("[Validation Layer] %s", callback_data->pMessage);
  else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    logger->debug("[Validation Layer] %s", callback_data->pMessage);
  return VK_FALSE;
}
#endif
