#include "Vulkan.inc"

/* <--- CALLBACKS ---> */
int me::Vulkan::callback_surface_resize(uint32_t width, uint32_t height, void* ptr)
{
  Vulkan* instance = reinterpret_cast<Vulkan*>(ptr);
  instance->render_info.flags |= RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
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
