#include "Util.hpp"

#include <string.h>

bool me::util::has_required_extensions(
    const array_proxy<VkExtensionProperties> 		&extensions,
    const array_proxy<const char*, uint32_t> 		&required_extensions
    )
{
  for (const char* required_extension : required_extensions)
  {
    bool has_extension = false;
    for (VkExtensionProperties extension : extensions)
    {
      if (strcmp(required_extension, extension.extensionName) == 0)
      {
	has_extension = true;
	break;
      }
    }

    if (!has_extension)
      return false;
  }
  return true;
}

bool me::util::has_required_layers(
    const array_proxy<VkLayerProperties>		&layers,
    const array_proxy<const char*, uint32_t>		&required_layers
    )
{
  for (const char* required_layer : required_layers)
  {
    bool has_layer = false;
    for (VkLayerProperties layer : layers)
    {
      if (strcmp(required_layer, layer.layerName))
      {
	has_layer = true;
	break;
      }
    }

    if (!has_layer)
      return false;
  }
  return true;
}

#define ENUMSTR(i, e) if (e == i) return #e;

const char* me::util::get_result_string(
    VkResult 						result
    )
{
  ENUMSTR(result, VK_SUCCESS);
  ENUMSTR(result, VK_NOT_READY);
  ENUMSTR(result, VK_TIMEOUT);
  ENUMSTR(result, VK_EVENT_SET);
  ENUMSTR(result, VK_EVENT_RESET);
  ENUMSTR(result, VK_INCOMPLETE);

  ENUMSTR(result, VK_SUBOPTIMAL_KHR);

  ENUMSTR(result, VK_THREAD_IDLE_KHR);
  ENUMSTR(result, VK_THREAD_DONE_KHR);
  ENUMSTR(result, VK_OPERATION_DEFERRED_KHR);
  ENUMSTR(result, VK_OPERATION_NOT_DEFERRED_KHR);
  ENUMSTR(result, VK_PIPELINE_COMPILE_REQUIRED_EXT);

  ENUMSTR(result, VK_ERROR_OUT_OF_HOST_MEMORY)
  ENUMSTR(result, VK_ERROR_OUT_OF_DEVICE_MEMORY)
  ENUMSTR(result, VK_ERROR_INITIALIZATION_FAILED)
  ENUMSTR(result, VK_ERROR_DEVICE_LOST)
  ENUMSTR(result, VK_ERROR_MEMORY_MAP_FAILED)
  ENUMSTR(result, VK_ERROR_LAYER_NOT_PRESENT)
  ENUMSTR(result, VK_ERROR_EXTENSION_NOT_PRESENT)
  ENUMSTR(result, VK_ERROR_FEATURE_NOT_PRESENT)
  ENUMSTR(result, VK_ERROR_INCOMPATIBLE_DRIVER)
  ENUMSTR(result, VK_ERROR_TOO_MANY_OBJECTS)
  ENUMSTR(result, VK_ERROR_FORMAT_NOT_SUPPORTED)
  ENUMSTR(result, VK_ERROR_FRAGMENTED_POOL)
  ENUMSTR(result, VK_ERROR_OUT_OF_POOL_MEMORY)
  ENUMSTR(result, VK_ERROR_SURFACE_LOST_KHR)
  ENUMSTR(result, VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
  ENUMSTR(result, VK_ERROR_OUT_OF_DATE_KHR)
  ENUMSTR(result, VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
  ENUMSTR(result, VK_ERROR_INVALID_SHADER_NV)
  ENUMSTR(result, VK_ERROR_OUT_OF_POOL_MEMORY)
  ENUMSTR(result, VK_ERROR_INVALID_EXTERNAL_HANDLE)
  ENUMSTR(result, VK_ERROR_FRAGMENTATION)
  ENUMSTR(result, VK_ERROR_INVALID_DEVICE_ADDRESS_EXT)
  ENUMSTR(result, VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
  ENUMSTR(result, VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
  ENUMSTR(result, VK_ERROR_UNKNOWN)
  return "UNKNOWN_RESULT";
}

#undef ENUMSTR

VkFormat me::util::get_vulkan_format(
    Format format
    )
{
  switch (format)
  {
    case FORMAT_32FLOAT:
      return VK_FORMAT_R32_SFLOAT;
    case FORMAT_VECTOR2_32FLOAT:
      return VK_FORMAT_R32G32_SFLOAT;
    case FORMAT_VECTOR3_32FLOAT:
      return VK_FORMAT_R32G32B32_SFLOAT;
    case FORMAT_VECTOR4_32FLOAT:
      return VK_FORMAT_R32G32B32A32_SFLOAT;
    default:
      return VK_FORMAT_MAX_ENUM;
  }
}
