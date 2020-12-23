#include <vulkan/vulkan_core.h>
struct QueueFamilyIndices {
  me::optional<uint32_t> graphics;
  me::optional<uint32_t> present;
};

struct InstanceInfo {
  VkInstance instance = VK_NULL_HANDLE;
};

struct PhysicalDeviceInfo {
  VkPhysicalDevice device = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties properties;
  VkArray<VkExtensionProperties> extensions;
  VkArray<VkLayerProperties> layers;
  VkArray<VkQueueFamilyProperties> queue_family_properties;
  VkPhysicalDeviceFeatures features;
  QueueFamilyIndices queue_family_indices;
};

struct LogicalDeviceInfo {
  VkDevice device = VK_NULL_HANDLE;
};

struct QueueInfo {
  VkQueue graphics_queue = VK_NULL_HANDLE;
  VkQueue present_queue = VK_NULL_HANDLE;
};

struct SurfaceInfo {
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR format;
  VkPresentModeKHR present_mode;
  VkExtent2D extent;
};

struct SwapchainInfo {
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkExtent2D image_extent;
  VkFormat image_format;
  VkColorSpaceKHR image_color_space;
  VkArray<VkImage> images;
  VkArray<VkImageView> image_views;
};

struct RenderPassInfo {
  VkRenderPass render_pass = VK_NULL_HANDLE;
};

struct PipelineLayoutInfo {
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
};

struct GraphicsPipelineInfo {
  VkPipeline pipeline;
};

struct FramebufferInfo {
  VkArray<VkFramebuffer> framebuffers;
};

struct CommandPoolInfo {
  VkCommandPool command_pool = VK_NULL_HANDLE;
};

struct SynchronizationInfo {
  VkArray<VkSemaphore> image_available;
  VkArray<VkSemaphore> render_finished;
  VkArray<VkFence> in_flight;
  VkArray<VkFence> images_in_flight;
};

struct ViewportInfo {
  VkArray<VkViewport> viewports;
  VkArray<VkRect2D> scissors;

  VkPipelineViewportStateCreateInfo pipline_viewport_stage_create_info;
};

struct RasterizerInfo {
  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info;
};

struct MultisamplingInfo {
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
};

struct ColorBlendInfo {
  VkArray<VkPipelineColorBlendAttachmentState> pipeline_color_blend_attachment_states;

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
};

struct DynamicInfo {
  VkArray<VkDynamicState> dynamics;

  VkPipelineDynamicStateCreateInfo pipline_dynamic_state_create_info;
};

struct ShaderInfo {
  vector<Shader*> shaders;
  vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos;
  VkPipelineVertexInputStateCreateInfo vertex_input_state;
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
};

struct RenderInfo {
  uint32_t frame_index;
};

struct Storage {
};

struct Temp {
};

#ifndef NDEBUG
struct DebugInfo {
  VkDebugReportCallbackEXT debug_report_callback;
  VkDebugUtilsMessengerEXT debug_utils_messenger;
};
#endif
