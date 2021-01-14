#include "Vulkan.inc"

#ifndef NDEBUG
/* <--- SETUP ---> */
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

/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_debug_messenger()
{
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkDestroyDebugUtilsMessengerEXT");

  vkDestroyDebugUtilsMessengerEXT(instance_info.instance, debug_info.debug_utils_messenger, nullptr);
  return 0;
}

int me::Vulkan::cleanup_debug_report()
{
  PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance_info.instance, "vkDestroyDebugReportCallbackEXT");

  vkDestroyDebugReportCallbackEXT(instance_info.instance, debug_info.debug_report_callback, nullptr);
  return 0;
}
#endif
