#include <vulkan/vulkan_core.h>

struct VertexInfo {
  VkVertexInputBindingDescription vertex_input_binding_description;
  me::array<VkVertexInputAttributeDescription, 4> vertex_input_attribute_descriptions;
};

struct MeshInfo {
};

struct QueueFamilyIndices {
  me::optional<uint32_t> graphics;
  me::optional<uint32_t> present;
  me::optional<uint32_t> transfer;
};

struct InstanceInfo {
  VkInstance instance = VK_NULL_HANDLE;
};

struct PhysicalDeviceInfo {
  VkPhysicalDevice device = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties properties;
  vector<VkExtensionProperties> extensions;
  vector<VkLayerProperties> layers;
  vector<VkQueueFamilyProperties> queue_family_properties;
  VkPhysicalDeviceFeatures features;
  QueueFamilyIndices queue_family_indices;
};

struct LogicalDeviceInfo {
  VkDevice device = VK_NULL_HANDLE;
};

struct QueueInfo {
  VkQueue graphics_queue = VK_NULL_HANDLE;
  VkQueue present_queue = VK_NULL_HANDLE;
  VkQueue transfer_queue = VK_NULL_HANDLE;
};

struct SurfaceInfo {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
};

struct SwapchainInfo {
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceFormatKHR surface_format;
  VkPresentModeKHR present_mode;
  VkExtent2D image_extent;
  VkFormat image_format;
  VkColorSpaceKHR image_color_space;
  vector<VkImage> images;
  vector<VkImageView> image_views;
};

struct RenderPassInfo {
  VkRenderPass render_pass = VK_NULL_HANDLE;
};

struct PipelineLayoutInfo {
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
};

struct GraphicsPipelineInfo {
  VkPipeline pipeline = VK_NULL_HANDLE;
};

struct FramebufferInfo {
  vector<VkFramebuffer> framebuffers;
};

struct CommandPoolInfo {
  VkCommandPool graphics_command_pool = VK_NULL_HANDLE;
  VkCommandPool transfer_command_pool = VK_NULL_HANDLE;
};

struct CommandBufferInfo {
  vector<VkCommandBuffer> draw_command_buffers;
};

struct SynchronizationInfo {
  vector<VkSemaphore> image_available;
  vector<VkSemaphore> render_finished;
  vector<VkFence> in_flight;
  vector<VkFence> images_in_flight;
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

struct ShaderInfo {
  vector<Shader*> shaders;
  vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos;
  VkVertexInputBindingDescription vertex_input_binding_descriptions[1];
  VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[1];
  VkVertexInputAttributeDescription vertex_input_attribute_descriptions[4];
  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
};

struct RenderInfo {

  enum Flag : uint8_t {
    FRAMEBUFFER_RESIZED_FLAG_BIT = 1
  };

  enum State : uint8_t {
    IDLE_STATE,
    ACTIVE_STATE,
    NO_SWAPCHAIN_STATE
  };

  uint8_t flags = 0;
  State state = IDLE_STATE;
  uint32_t frame_index = 0;
};

#ifndef NDEBUG
struct DebugInfo {
  VkDebugReportCallbackEXT debug_report_callback;
  VkDebugUtilsMessengerEXT debug_utils_messenger;
};
#endif

struct MemoryInfo {
  VkDeviceMemory vertex_buffer_memory;
  VkDeviceMemory index_buffer_memory;
  VkDeviceMemory staging_buffer_memory;
};

struct DataStorage {
  vector<Mesh*> meshes;
};
