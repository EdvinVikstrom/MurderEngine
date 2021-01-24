#ifndef NDEBUG

#include "Vulkan.hpp"
#include "Util.hpp"

static int create_debug_messenger(
    VkInstance 						instance,
    VkAllocationCallbacks* 				allocation,
    me::Logger* 					logger,
    PFN_vkDebugUtilsMessengerCallbackEXT		callback,
    VkDebugUtilsMessengerEXT 				&debug_utils_messenger
    );

static int create_debug_report(
    VkInstance 						instance,
    VkAllocationCallbacks* 				allocation,
    me::Logger* 					logger,
    PFN_vkDebugReportCallbackEXT			callback,
    VkDebugReportCallbackEXT 				&debug_report_callback
    );


int me::vulkan::Vulkan::setup_debug()
{
  create_debug_messenger(vk_instance, vk_allocation, &logger, debug_callback, vk_debug_utils_messenger);
  create_debug_report(vk_instance, vk_allocation, &logger, debug_callback, vk_debug_report_callback);
  return 0;
}

VkBool32 me::vulkan::Vulkan::debug_callback(
    VkDebugReportFlagsEXT 				debug_report_flags,
    VkDebugReportObjectTypeEXT 				debug_report_object_type,
    uint64_t 						object,
    size_t 						location,
    int32_t 						message_code,
    const char* 					layer_prefix,
    const char* 					message,
    void* 						user_data
    )
{
  me::Logger* logger = (me::Logger*) user_data;
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

VkBool32 me::vulkan::Vulkan::debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT 		message_severity,
    VkDebugUtilsMessageTypeFlagsEXT 			message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* 	callback_data, 
    void* 						user_data
    )
{
  me::Logger* logger = (me::Logger*) user_data;
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

int me::vulkan::Vulkan::cleanup_debug()
{
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT(vk_instance, vk_debug_utils_messenger, vk_allocation);

  PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
    vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugReportCallbackEXT");
  vkDestroyDebugReportCallbackEXT(vk_instance, vk_debug_report_callback, vk_allocation);
  return 0;
}


int create_debug_messenger(
    VkInstance 						instance,
    VkAllocationCallbacks* 				allocation,
    me::Logger* 					logger,
    PFN_vkDebugUtilsMessengerCallbackEXT		callback,
    VkDebugUtilsMessengerEXT 				&debug_utils_messenger
    )
{
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

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
  debug_utils_messenger_create_info.pfnUserCallback = callback;
  debug_utils_messenger_create_info.pUserData = logger;

  VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &debug_utils_messenger_create_info,
      allocation, &debug_utils_messenger);

  if (result != VK_SUCCESS)
    throw me::exception("failed to create debug utils messenger [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}

int create_debug_report(
    VkInstance 						instance,
    VkAllocationCallbacks* 				allocation,
    me::Logger* 					logger,
    PFN_vkDebugReportCallbackEXT			callback,
    VkDebugReportCallbackEXT 				&debug_report_callback
    )
{
  PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

  VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info = { };
  debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debug_report_callback_create_info.pNext = nullptr;
  debug_report_callback_create_info.flags =
    VK_DEBUG_REPORT_ERROR_BIT_EXT |
    VK_DEBUG_REPORT_WARNING_BIT_EXT |
    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
    VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
    VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  debug_report_callback_create_info.pfnCallback = callback;
  debug_report_callback_create_info.pUserData = logger;

  VkResult result = vkCreateDebugReportCallbackEXT(instance, &debug_report_callback_create_info,
      allocation, &debug_report_callback);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create debug report callback [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}

#endif
