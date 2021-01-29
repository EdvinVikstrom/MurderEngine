#include "Vulkan.hpp"
#include "Util.hpp"
#include "Memory.hpp"

static int create_vertex_input(
    const me::array_proxy<me::ShaderBindingInfo, uint32_t> 		&binding_infos,
    const me::array_proxy<me::ShaderAttributeInfo, uint32_t> 		&attribute_infos,
    VkVertexInputBindingDescription* 					vertex_input_binding_descriptions,
    VkVertexInputAttributeDescription* 					vertex_input_attribute_descriptions,
    VkPipelineVertexInputStateCreateInfo 				&pipeline_vertex_input_state_create_info
    );

static int create_input_assembly(
    me::Topology 							topology,
    VkPipelineInputAssemblyStateCreateInfo 				&pipeline_input_assembly_state_create_info
    );

static int create_descriptor_set_layout(
    VkDevice 								device,
    VkAllocationCallbacks* 						allocation,
    const me::array_proxy<VkDescriptorSetLayoutBinding, uint32_t> 	&bindings,
    VkDescriptorSetLayout 						&descriptor_set_layout
    );

static int create_pipeline_layout(
    VkDevice 								device,
    VkAllocationCallbacks*						allocation,
    const me::array_proxy<VkDescriptorSetLayout, uint32_t>		&descriptor_set_layouts,
    VkPipelineLayout 							&pipeline_layout
    );

static int create_viewports(
    const me::array_proxy<me::ViewportInfo, uint32_t>			&viewport_infos,
    const me::array_proxy<me::ScissorInfo, uint32_t>			&scissor_infos,
    VkViewport*								viewports,
    VkRect2D*								scissors,
    VkPipelineViewportStateCreateInfo 					&pipeline_viewport_state_create_info
    );

static int create_rasterizer(
    const me::RasterizerCreateInfo 					&rasterizer_info,
    VkPipelineRasterizationStateCreateInfo 				&pipeline_rasterization_state_create_info
    );

static int create_multisampling(
    const me::MultisamplingCreateInfo 					&multisampling_info,
    VkPipelineMultisampleStateCreateInfo 				&pipeline_multisample_state_create_info
    );

static int create_color_blend(
    VkPipelineColorBlendAttachmentState* 				pipeline_color_blend_attachment_states,
    VkPipelineColorBlendStateCreateInfo 				&vk_pipeline_color_blend_state_create_info
    );

static int create_shader_module(
    VkDevice 								device,
    VkAllocationCallbacks*						allocation,
    const me::Shader* 							shader,
    VkShaderModule 							&shader_module,
    VkPipelineShaderStageCreateInfo 					&pipeline_shader_stage_create_info
    );


int me::Vulkan::create_pipeline(const PipelineCreateInfo &pipeline_create_info, Pipeline &pipeline)
{
  VERIFY_CREATE_INFO(pipeline_create_info, STRUCTURE_TYPE_PIPELINE_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(pipeline_create_info.device)->vk_device;
  VulkanRenderPass* render_pass = reinterpret_cast<VulkanRenderPass*>(pipeline_create_info.render_pass);

  ShaderCreateInfo &shader_create_info = *pipeline_create_info.shader_create_info;
  RasterizerCreateInfo &rasterizer_create_info = *pipeline_create_info.rasterizer_create_info;
  MultisamplingCreateInfo &multisampling_create_info = *pipeline_create_info.multisampling_create_info;

  /* creating vertex input */
  VkVertexInputBindingDescription vertex_input_binding_descriptions[shader_create_info.vertex_binding_count];
  VkVertexInputAttributeDescription vertex_input_attribute_descriptions[shader_create_info.vertex_attribute_count];
  VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info;
  create_vertex_input(
      {shader_create_info.vertex_binding_count, shader_create_info.vertex_bindings},
      {shader_create_info.vertex_attribute_count, shader_create_info.vertex_attributes},
      vertex_input_binding_descriptions, vertex_input_attribute_descriptions, pipeline_vertex_input_state_create_info);

  /* creating assembly input */
  VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info;
  create_input_assembly(shader_create_info.topology, pipeline_input_assembly_state_create_info);

  /* uniform buffer object */
  uint32_t descriptor_set_layout_binding_count = 1;
  VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[descriptor_set_layout_binding_count];
  descriptor_set_layout_bindings[0].binding = 0;
  descriptor_set_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_set_layout_bindings[0].descriptorCount = 1;
  descriptor_set_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  descriptor_set_layout_bindings[0].pImmutableSamplers = nullptr;

  /* creating descriptor set layouts */
  VkDescriptorSetLayout vk_descriptor_set_layout;
  create_descriptor_set_layout(vk_device, vk_allocation,
      {descriptor_set_layout_binding_count, descriptor_set_layout_bindings}, vk_descriptor_set_layout);

  /* creating pipeline layout */
  VkPipelineLayout vk_layout;
  create_pipeline_layout(vk_device, vk_allocation,
      {1, &vk_descriptor_set_layout}, vk_layout);

  /* creating viewports and scissors */
  VkViewport viewports[pipeline_create_info.viewport_count];
  VkRect2D scissors[pipeline_create_info.scissor_count];
  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info;
  create_viewports(
      {pipeline_create_info.viewport_count, pipeline_create_info.viewports},
      {pipeline_create_info.scissor_count, pipeline_create_info.scissors},
      viewports, scissors, pipeline_viewport_state_create_info);

  /* creating rasterizer */
  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info;
  create_rasterizer(rasterizer_create_info, pipeline_rasterization_state_create_info);

  /* creating multisampling */
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
  create_multisampling(multisampling_create_info, pipeline_multisample_state_create_info);

  /* creating color blend */
  VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_states[1];
  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
  create_color_blend(pipeline_color_blend_attachment_states, pipeline_color_blend_state_create_info);


  /* creating shader modules */
  VkShaderModule shader_modules[shader_create_info.shader_count];
  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_infos[shader_create_info.shader_count];
  for (uint32_t i = 0; i < shader_create_info.shader_count; i++)
    create_shader_module(vk_device, vk_allocation, shader_create_info.shaders[i], shader_modules[i], pipeline_shader_stage_create_infos[i]);


  /* === Creating graphics pipeline === */
  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = { };
  graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_create_info.pNext = nullptr;
  graphics_pipeline_create_info.flags = 0;
  graphics_pipeline_create_info.stageCount = shader_create_info.shader_count;
  graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_infos;
  graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
  graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
  graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
  graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_create_info;
  graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
  graphics_pipeline_create_info.pDepthStencilState = nullptr;
  graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
  graphics_pipeline_create_info.pDynamicState = nullptr;
  graphics_pipeline_create_info.layout = vk_layout;
  graphics_pipeline_create_info.renderPass = render_pass->vk_render_pass;
  graphics_pipeline_create_info.subpass = 0;
  graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  graphics_pipeline_create_info.basePipelineIndex = -1;

  VkPipeline vk_pipeline;
  VkResult result = vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, vk_allocation, &vk_pipeline);
  if (result != VK_SUCCESS)
    throw exception("failed to create graphics piplines [%s]", util::get_result_string(result));

  for (uint32_t i = 0; i < shader_create_info.shader_count; i++)
    vkDestroyShaderModule(vk_device, shader_modules[i], vk_allocation);

  pipeline = alloc.allocate<VulkanPipeline>(vk_pipeline, vk_layout, vk_descriptor_set_layout);
  return 0;
}

/*
int me::vulkan::Vulkan::setup_uniform_buffer(const UniformBufferInfo &uniform_buffer_info, UniformBuffer &_uniform_buffer)
{
  for (uint32_t i = 0; i < swapchain_images.size(); i++)
  {
    SwapchainImage &swapchain_image = swapchain_images[i];

    VkDeviceSize buffer_size = uniform_buffer_info.size;

    VkBuffer buffer;
    VkDeviceMemory buffer_memory;
    memory::create_buffer(vk_physical_device, vk_device, buffer_size,
	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	buffer, buffer_memory);

    _uniform_buffer = uniform_buffer_infos.size();
    uniform_buffer_infos.push_back(uniform_buffer_info);
    swapchain_image.vk_uniform_buffers.push_back(buffer);
    swapchain_image.vk_uniform_buffers_memory.push_back(buffer_memory);
  }
  return 0;
}
*/

int me::Vulkan::create_descriptors(const DescriptorCreateInfo &descriptor_create_info,
    uint32_t descriptor_count, Descriptor* descriptors)
{
  VERIFY_CREATE_INFO(descriptor_create_info, STRUCTURE_TYPE_DESCRIPTOR_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(descriptor_create_info.device)->vk_device;
  VulkanDescriptorPool* descriptor_pool = reinterpret_cast<VulkanDescriptorPool*>(descriptor_create_info.descriptor_pool);
  VulkanPipeline* pipeline = reinterpret_cast<VulkanPipeline*>(descriptor_create_info.pipeline);

  if (descriptor_create_info.descriptor_type != descriptor_pool->type)
    throw exception("in 'create_descriptors()' DescriptorCreateInfo::descriptor_type must be the same as DescriptorPool::descriptor_type. %s != \e[33m%s\e[0m",
	descriptor_type_name(descriptor_create_info.descriptor_type), descriptor_type_name(descriptor_pool->type));

  if (descriptor_create_info.buffer_count != descriptor_count)
    throw exception("in 'create_descriptors()' DescriptorCreateInfo::buffer_count must be the same as 'descriptor_count'. %u != \e[33m%u\e[0m",	
	descriptor_create_info.buffer_count, descriptor_count);

  uint32_t descriptor_set_layout_count = 1;
  VkDescriptorSetLayout vk_descriptor_set_layouts[descriptor_set_layout_count];
  vk_descriptor_set_layouts[0] = pipeline->vk_descriptor_set_layout;

  VkDescriptorSetAllocateInfo vk_descriptor_set_allocate_info = { };
  vk_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  vk_descriptor_set_allocate_info.pNext = nullptr;
  vk_descriptor_set_allocate_info.descriptorPool = descriptor_pool->vk_descriptor_pool;
  vk_descriptor_set_allocate_info.descriptorSetCount = descriptor_set_layout_count;
  vk_descriptor_set_allocate_info.pSetLayouts = vk_descriptor_set_layouts;

  VkDescriptorSet vk_descriptor_sets[descriptor_count];
  VkResult result = vkAllocateDescriptorSets(vk_device, &vk_descriptor_set_allocate_info, vk_descriptor_sets);
  if (result != VK_SUCCESS)
    throw exception("failed to allocate descriptor sets [%s]", util::get_result_string(result));

  for (uint32_t i = 0; i < descriptor_count; i++)
  {
    VulkanBuffer* buffer = reinterpret_cast<VulkanBuffer*>(&descriptor_create_info.buffers[i]);

    VkDescriptorBufferInfo vk_descriptor_buffer_info = { };
    vk_descriptor_buffer_info.buffer = buffer->vk_buffer;
    vk_descriptor_buffer_info.offset = descriptor_create_info.offset;
    vk_descriptor_buffer_info.range = descriptor_create_info.range;

    VkWriteDescriptorSet vk_write_descriptor_set = { };
    vk_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vk_write_descriptor_set.pNext = nullptr;
    vk_write_descriptor_set.dstSet = vk_descriptor_sets[i];
    vk_write_descriptor_set.dstBinding = 0;
    vk_write_descriptor_set.dstArrayElement = 0;
    vk_write_descriptor_set.descriptorCount = 1;
    vk_write_descriptor_set.descriptorType = util::get_vulkan_descriptor_type(descriptor_create_info.descriptor_type);
    vk_write_descriptor_set.pImageInfo = nullptr;
    vk_write_descriptor_set.pBufferInfo = &vk_descriptor_buffer_info;
    vk_write_descriptor_set.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(vk_device, 1, &vk_write_descriptor_set, 0, nullptr);

    descriptors[i] = alloc.allocate<VulkanDescriptor>(vk_descriptor_sets[i]);
  }
  return 0;
}

int me::Vulkan::cleanup_pipeline(const PipelineCleanupInfo &pipeline_cleanup_info, Pipeline pipeline_ptr)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(pipeline_cleanup_info.device)->vk_device;
  VulkanPipeline* pipeline = reinterpret_cast<VulkanPipeline*>(pipeline_ptr);

  vkDestroyDescriptorSetLayout(vk_device, pipeline->vk_descriptor_set_layout, vk_allocation);
  vkDestroyPipelineLayout(vk_device, pipeline->vk_layout, vk_allocation);
  vkDestroyPipeline(vk_device, pipeline->vk_pipeline, vk_allocation);
  return 0;
}

int me::Vulkan::cleanup_descriptors(const DescriptorCleanupInfo &descriptor_cleanup_info, uint32_t descriptor_count, Descriptor* descriptors)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(descriptor_cleanup_info.device)->vk_device;
  VulkanDescriptorPool* descriptor_pool = reinterpret_cast<VulkanDescriptorPool*>(descriptor_cleanup_info.descriptor_pool);

  VkDescriptorSet vk_descriptor_sets[descriptor_count];
  for (uint32_t i = 0; i < descriptor_count; i++)
  {
    VulkanDescriptor* descriptor = reinterpret_cast<VulkanDescriptor*>(descriptors[i]);
    vk_descriptor_sets[i] = descriptor->vk_descriptor_set;
  }

  vkFreeDescriptorSets(vk_device, descriptor_pool->vk_descriptor_pool, descriptor_count, vk_descriptor_sets);
  return 0;
}


int create_vertex_input(
    const me::array_proxy<me::ShaderBindingInfo, uint32_t> 		&binding_infos,
    const me::array_proxy<me::ShaderAttributeInfo, uint32_t> 		&attribute_infos,
    VkVertexInputBindingDescription* 					vertex_input_binding_descriptions,
    VkVertexInputAttributeDescription* 					vertex_input_attribute_descriptions,
    VkPipelineVertexInputStateCreateInfo 				&pipeline_vertex_input_state_create_info
    )
{
  for (uint32_t i = 0; i < binding_infos.size(); i++)
  {
    const me::ShaderBindingInfo &binding_info = binding_infos[i];
    vertex_input_binding_descriptions[i].binding = binding_info.binding;
    vertex_input_binding_descriptions[i].stride = binding_info.stride;
    vertex_input_binding_descriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  }

  for (uint32_t i = 0; i < attribute_infos.size(); i++)
  {
    const me::ShaderAttributeInfo &attribute_info = attribute_infos[i];
    VkFormat format = me::util::get_vulkan_format(attribute_info.format);

    vertex_input_attribute_descriptions[i].binding = attribute_info.binding;
    vertex_input_attribute_descriptions[i].location = attribute_info.location;
    vertex_input_attribute_descriptions[i].format = format;
    vertex_input_attribute_descriptions[i].offset = attribute_info.offset;
  }

  pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipeline_vertex_input_state_create_info.pNext = nullptr;
  pipeline_vertex_input_state_create_info.flags = 0;
  pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = binding_infos.size();
  pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = vertex_input_binding_descriptions;
  pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_infos.size(); 
  pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_input_attribute_descriptions;
  return 0;
}

int create_input_assembly(
    me::Topology 							topology,
    VkPipelineInputAssemblyStateCreateInfo 				&pipeline_input_assembly_state_create_info
    )
{
  VkPrimitiveTopology vk_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  if (topology == me::TOPOLOGY_TRIANGLE_LIST)
    vk_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  pipeline_input_assembly_state_create_info.pNext = nullptr;
  pipeline_input_assembly_state_create_info.flags = 0;
  pipeline_input_assembly_state_create_info.topology = vk_topology;
  pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;
  return 0;
}

int create_descriptor_set_layout(
    VkDevice 								device,
    VkAllocationCallbacks* 						allocation,
    const me::array_proxy<VkDescriptorSetLayoutBinding, uint32_t> 	&bindings,
    VkDescriptorSetLayout 						&descriptor_set_layout
    )
{
  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = { };
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pNext = nullptr;
  descriptor_set_layout_create_info.bindingCount = bindings.size();
  descriptor_set_layout_create_info.pBindings = bindings.data();

  VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, allocation, &descriptor_set_layout);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create descriptor set layout [%s]", me::util::get_result_string(result));
  return 0;
}

int create_pipeline_layout(
    VkDevice 								device,
    VkAllocationCallbacks* 						allocation,
    const me::array_proxy<VkDescriptorSetLayout, uint32_t>		&descriptor_set_layouts,
    VkPipelineLayout 							&pipeline_layout
    )
{
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = { };
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.pNext = nullptr;
  pipeline_layout_create_info.flags = 0;
  pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
  pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
  pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.pPushConstantRanges = nullptr;
 
  VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_create_info, allocation, &pipeline_layout);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create pipeline layout [%s]", me::util::get_result_string(result));
  return 0;
}

int create_viewports(
    const me::array_proxy<me::ViewportInfo, uint32_t>			&viewport_infos,
    const me::array_proxy<me::ScissorInfo, uint32_t>			&scissor_infos,
    VkViewport*								viewports,
    VkRect2D*								scissors,
    VkPipelineViewportStateCreateInfo 					&pipeline_viewport_state_create_info
    )
{
  for (uint32_t i = 0; i < viewport_infos.size(); i++)
  {
    const me::ViewportInfo &viewport_info = viewport_infos[i];
    viewports[i] = {
        .x = viewport_info.location[0],
        .y = viewport_info.location[1],
        .width = viewport_info.size[0],
        .height = viewport_info.size[1],
        .minDepth = 0.0F,
	.maxDepth = 1.0F
    };

    const me::ScissorInfo &scissor_info = scissor_infos[i];
    scissors[i] = {
        .offset = {scissor_info.offset[0], scissor_info.offset[1]},
        .extent = {scissor_info.size[0], scissor_info.size[1]}
    };
  }

  pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  pipeline_viewport_state_create_info.pNext = nullptr;
  pipeline_viewport_state_create_info.flags = 0;
  pipeline_viewport_state_create_info.viewportCount = viewport_infos.size();
  pipeline_viewport_state_create_info.pViewports = viewports;
  pipeline_viewport_state_create_info.scissorCount = scissor_infos.size();
  pipeline_viewport_state_create_info.pScissors = scissors;
  return 0;
}

int create_rasterizer(
    const me::RasterizerCreateInfo 					&rasterizer_info,
    VkPipelineRasterizationStateCreateInfo 				&pipeline_rasterization_state_create_info
    )
{
  VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
  VkCullModeFlagBits cull_mode = VK_CULL_MODE_BACK_BIT;
  VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  if (rasterizer_info.polygon_mode == me::POLYGON_MODE_FILL)
    polygon_mode = VK_POLYGON_MODE_FILL;

  if (rasterizer_info.cull_mode == me::CULL_MODE_BACK)
    cull_mode = VK_CULL_MODE_BACK_BIT;
  else if (rasterizer_info.cull_mode == me::CULL_MODE_FRONT)
    cull_mode = VK_CULL_MODE_FRONT_BIT;
  else if (rasterizer_info.cull_mode == me::CULL_MODE_FRONT_BACK)
    cull_mode = VK_CULL_MODE_FRONT_AND_BACK;

  if (rasterizer_info.front_face == me::FRONT_FACE_CLOCKWISE)
    front_face = VK_FRONT_FACE_CLOCKWISE;
  else if (rasterizer_info.front_face == me::FRONT_FACE_COUNTER_CLOCKWISE)
    front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  pipeline_rasterization_state_create_info.pNext = nullptr;
  pipeline_rasterization_state_create_info.flags = 0;
  pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
  pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
  pipeline_rasterization_state_create_info.polygonMode = polygon_mode;
  pipeline_rasterization_state_create_info.cullMode = cull_mode;
  pipeline_rasterization_state_create_info.frontFace = front_face;
  pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
  pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0F;
  pipeline_rasterization_state_create_info.depthBiasClamp = 0.0F;
  pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0F;
  pipeline_rasterization_state_create_info.lineWidth = 1.0F;
  return 0;
}

int create_multisampling(
    const me::MultisamplingCreateInfo 					&multisampling_info,
    VkPipelineMultisampleStateCreateInfo 				&pipeline_multisample_state_create_info
    )
{
  VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;

  if (multisampling_info.samples == me::SAMPLE_COUNT_1)
    sample_count = VK_SAMPLE_COUNT_1_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_2)
    sample_count = VK_SAMPLE_COUNT_2_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_4)
    sample_count = VK_SAMPLE_COUNT_4_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_8)
    sample_count = VK_SAMPLE_COUNT_8_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_16)
    sample_count = VK_SAMPLE_COUNT_16_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_32)
    sample_count = VK_SAMPLE_COUNT_32_BIT;
  else if (multisampling_info.samples == me::SAMPLE_COUNT_64)
    sample_count = VK_SAMPLE_COUNT_64_BIT;

  pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  pipeline_multisample_state_create_info.pNext = nullptr;
  pipeline_multisample_state_create_info.flags = 0;
  pipeline_multisample_state_create_info.rasterizationSamples = sample_count;
  pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
  pipeline_multisample_state_create_info.minSampleShading = 1.0F;
  pipeline_multisample_state_create_info.pSampleMask = nullptr;
  pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
  pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;
  return 0;
}

int create_color_blend(
    VkPipelineColorBlendAttachmentState* 				pipeline_color_blend_attachment_states,
    VkPipelineColorBlendStateCreateInfo 				&pipeline_color_blend_state_create_info
    )
{
  pipeline_color_blend_attachment_states[0] = {
    VK_FALSE,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };

  pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  pipeline_color_blend_state_create_info.pNext = nullptr;
  pipeline_color_blend_state_create_info.flags = 0;
  pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
  pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  pipeline_color_blend_state_create_info.attachmentCount = 1;
  pipeline_color_blend_state_create_info.pAttachments = pipeline_color_blend_attachment_states;
  pipeline_color_blend_state_create_info.blendConstants[0] = 0.0F;
  pipeline_color_blend_state_create_info.blendConstants[1] = 0.0F;
  pipeline_color_blend_state_create_info.blendConstants[2] = 0.0F;
  pipeline_color_blend_state_create_info.blendConstants[3] = 0.0F;
  return 0;
}

int create_shader_module(
    VkDevice 								device,
    VkAllocationCallbacks* 						allocation,
    const me::Shader* 							shader,
    VkShaderModule 							&shader_module,
    VkPipelineShaderStageCreateInfo 					&pipeline_shader_stage_create_info
    )
{
  VkShaderModuleCreateInfo shader_create_info = { };
  shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_create_info.pNext = nullptr;
  shader_create_info.flags = 0;
  shader_create_info.codeSize = shader->get_length();
  shader_create_info.pCode = reinterpret_cast<const uint32_t*>(shader->get_data());

  VkResult result = vkCreateShaderModule(device, &shader_create_info, allocation, &shader_module);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create shader module [%s]", me::util::get_result_string(result));

  VkShaderStageFlagBits shader_stage_flag_bits;
  switch (shader->get_type())
  {
    case me::SHADER_VERTEX: shader_stage_flag_bits = VK_SHADER_STAGE_VERTEX_BIT; break;
    case me::SHADER_FRAGMENT: shader_stage_flag_bits = VK_SHADER_STAGE_FRAGMENT_BIT; break;
    case me::SHADER_GEOMETRY: shader_stage_flag_bits = VK_SHADER_STAGE_GEOMETRY_BIT; break;
  }

  pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  pipeline_shader_stage_create_info.pNext = nullptr;
  pipeline_shader_stage_create_info.flags = 0;
  pipeline_shader_stage_create_info.stage = shader_stage_flag_bits;
  pipeline_shader_stage_create_info.module = shader_module;
  pipeline_shader_stage_create_info.pName = shader->get_config().entry_point;
  pipeline_shader_stage_create_info.pSpecializationInfo = nullptr;
  return 0;
}
