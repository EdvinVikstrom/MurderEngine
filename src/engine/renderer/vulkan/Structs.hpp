struct QueueFamilyIndices {
  me::optional<uint32_t> graphics;
};

struct InstanceInfo {
  VkInstance instance;
};

struct InstanceCreateInfo {
  VkInstanceCreateInfo info;
  VkApplicationInfo application_info;
  vector<const char*> extensions;
};

struct PhysicalDeviceInfo {
  VkPhysicalDevice device;
  VkPhysicalDeviceProperties properties;
  vector<VkExtensionProperties> extensions;
  VkPhysicalDeviceFeatures features;
  QueueFamilyIndices queue_family_indices;
};

struct PhysicalDeviceCreateInfo {
  vector<PhysicalDeviceInfo> physical_device_infos;
  vector<const char*> required_extensions;
  int required_flags;
};

struct LogicalDeviceInfo {
  VkDevice device;
};

struct LogicalDeviceCreateInfo {
  VkDeviceCreateInfo info;
  vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
  vector<const char*> extension_names;
};

struct CommandPoolInfo {
  VkCommandPool command_pool;
};

struct CommandPoolCreateInfo {
  VkCommandPoolCreateInfo info;
  VkDevice device;
};

struct SurfaceInfo {
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR format;
  VkPresentModeKHR present_mode;
  VkExtent2D extent;
};

struct SurfaceCreateInfo {
  vector<VkPresentModeKHR> present_modes;
  vector<VkSurfaceFormatKHR> formats;
};

struct SwapchainInfo {
  VkSwapchainKHR swapchain;
  VkExtent2D image_extent;
  VkFormat image_format;
  VkColorSpaceKHR image_color_space;
  vector<VkImage> images;
  vector<VkImageView> image_views;
};

struct SwapchainCreateInfo {
  VkSwapchainCreateInfoKHR info;
  VkDevice device;
  uint32_t min_image_count;
  VkSurfaceKHR surface;
  VkSurfaceTransformFlagBitsKHR pre_transform;
  VkPresentModeKHR present_mode;
};

struct PipelineLayoutInfo {
  VkPipelineLayout pipeline_layout;
};

struct PipelineLayoutCreateInfo {
  VkPipelineLayoutCreateInfo info;
  VkDevice device;
};

struct RenderPassInfo {
  VkRenderPass render_pass;
};

struct RenderPassCreateInfo {
  VkRenderPassCreateInfo info;
  VkAttachmentDescription attachment_description;
};

struct GraphicsPipelineInfo {
};

struct GraphicsPipelineCreateInfo {
};

struct ViewportInfo {
  vector<VkViewport> viewports;
  vector<VkRect2D> scissors;

  VkPipelineViewportStateCreateInfo pipline_viewport_stage_create_info;
};

struct RasterizerInfo {
  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info;
};

struct MultisamplingInfo {
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
};

struct ColorBlendInfo {
  vector<VkPipelineColorBlendAttachmentState> pipeline_color_blend_attachment_states;

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
};

struct DynamicInfo {
  vector<VkDynamicState> dynamics;

  VkPipelineDynamicStateCreateInfo pipline_dynamic_state_create_info;
};

struct Storage {
  vector<VkShaderModule> shaders;
};

struct Temp {
  vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos;
};
