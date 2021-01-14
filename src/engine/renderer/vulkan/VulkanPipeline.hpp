#include "Vulkan.inc"
#include <vulkan/vulkan_core.h>

/* <--- SETUP ---> */
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
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
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

  shader_info.vertex_input_binding_descriptions[0].binding = 0;
  shader_info.vertex_input_binding_descriptions[0].stride = sizeof(Vertex);
  shader_info.vertex_input_binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  /* position */
  shader_info.vertex_input_attribute_descriptions[0].binding = 0;
  shader_info.vertex_input_attribute_descriptions[0].location = 0;
  shader_info.vertex_input_attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  shader_info.vertex_input_attribute_descriptions[0].offset = offsetof(Vertex, position);

  /* normal */
  shader_info.vertex_input_attribute_descriptions[1].binding = 0;
  shader_info.vertex_input_attribute_descriptions[1].location = 1;
  shader_info.vertex_input_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  shader_info.vertex_input_attribute_descriptions[1].offset = offsetof(Vertex, normal);

  /* texture */
  shader_info.vertex_input_attribute_descriptions[2].binding = 0;
  shader_info.vertex_input_attribute_descriptions[2].location = 2;
  shader_info.vertex_input_attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  shader_info.vertex_input_attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);

  /* color */
  shader_info.vertex_input_attribute_descriptions[3].binding = 0;
  shader_info.vertex_input_attribute_descriptions[3].location = 3;
  shader_info.vertex_input_attribute_descriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  shader_info.vertex_input_attribute_descriptions[3].offset = offsetof(Vertex, color);


  shader_info.vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  shader_info.vertex_input_state_create_info.pNext = nullptr;
  shader_info.vertex_input_state_create_info.flags = 0;
  shader_info.vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
  shader_info.vertex_input_state_create_info.pVertexBindingDescriptions = shader_info.vertex_input_binding_descriptions;
  shader_info.vertex_input_state_create_info.vertexAttributeDescriptionCount = 4; 
  shader_info.vertex_input_state_create_info.pVertexAttributeDescriptions = shader_info.vertex_input_attribute_descriptions;

  shader_info.input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  shader_info.input_assembly_state_create_info.pNext = nullptr;
  shader_info.input_assembly_state_create_info.flags = 0;
  shader_info.input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  shader_info.input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;
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
  graphics_pipeline_create_info.pVertexInputState = &shader_info.vertex_input_state_create_info;
  graphics_pipeline_create_info.pInputAssemblyState = &shader_info.input_assembly_state_create_info;
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


/* <--- HELPERS ---> */
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


/* <--- CLEANUP ---> */
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


/* <--- SETUP ---> */
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
