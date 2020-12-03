#include "VulkanRenderer.hpp"
#include "Util.hpp"

#include "Alloc_Impl.hpp"
#include "Device_Impl.hpp"
#include "Command_Impl.hpp"

me::VulkanRenderer::VulkanRenderer(const MurderEngine* engine, const Surface &surface)
  : Renderer(engine, "vulkan"), surface(surface), memory(1024)
{
}

int me::VulkanRenderer::vk_error(const VkResult result)
{
  throw Exception(logger->get_prefix(), true, "vulkan returned result [%s]", get_vulkan_result_string(result));
  return 0;
}

int me::VulkanRenderer::signal()
{
  return 0;
}

int me::VulkanRenderer::initialize()
{
  /* make a logger */
  logger = engine->get_logger().child("Vulkan");
  logger->trace_all();

  for (const char* ext : surface.get_extensions())
    extensions.push_back(ext);

  /* get vulkan api version */
  uint32_t api_version;
  VkResult result = vkEnumerateInstanceVersion(&api_version);
  if (result != VK_SUCCESS)
    vk_error(result);

  logger->debug("using api version [%u.%u.%u]", 
      VK_VERSION_MAJOR(api_version), VK_VERSION_MINOR(api_version), VK_VERSION_PATCH(api_version));


  /* application info */
  const AppConfig &app_config = engine->get_app_config();
  const VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = app_config.name,
    .applicationVersion = app_config.version,
    .pEngineName = ME_ENGINE_NAME,
    .engineVersion = VK_MAKE_VERSION(ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH),
    .apiVersion = VK_MAKE_VERSION(1, 2, 0)
  };

  /* instance info */
  const VkInstanceCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pApplicationInfo = &app_info,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = (uint32_t) extensions.size(),
    .ppEnabledExtensionNames = extensions.data()
  };

  /* allocation callbacks */
  alloc_callbacks = {
    .pUserData = &memory,
    .pfnAllocation = vk_allocate,
    .pfnReallocation = vk_reallocate,
    .pfnFree = vk_free,
    .pfnInternalAllocation = vk_iallocaten,
    .pfnInternalFree = vk_ifreen
  };


  /* create a vulkan instance */
  result = vkCreateInstance(&create_info, &alloc_callbacks, &instance);
  if (result != VK_SUCCESS)
    vk_error(result);

  init_devices();
  init_commands();

  return 0;
}

int me::VulkanRenderer::terminate()
{
  free_command_buffers();
  vkDestroyCommandPool(device, command_pool, &alloc_callbacks);
  free_devices();
  vkDestroyInstance(instance, &alloc_callbacks);
  return 0;
}

int me::VulkanRenderer::tick()
{
  /*
  VkSubmitInfo submit_info = {

  };

  vkQueueSubmit(queues.graphics_queues.at(0));
  */
  return 0;
}
