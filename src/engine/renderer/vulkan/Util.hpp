#ifndef ME_VULKAN_UTIL_HPP
  #define ME_VULKAN_UTIL_HPP

#include "../Renderer.hpp"

#include <lme/array_proxy.hpp>

#include <vulkan/vulkan.h>

namespace me::util {

  bool has_required_extensions(
      const array_proxy<VkExtensionProperties> 		&extensions,
      const array_proxy<const char*> 			&required_extensions
      );

  bool has_required_layers(
      const array_proxy<VkLayerProperties>		&layers,
      const array_proxy<const char*>			&required_layers
      );

  const char* get_result_string(
      VkResult 						result
      );

  VkPhysicalDeviceType get_vulkan_physical_device_type(
      PhysicalDeviceType physical_device_type
      );

  PhysicalDeviceType get_physical_device_type(
      VkPhysicalDeviceType vk_physical_device_type
      );

  VkFormat get_vulkan_format(
      Format format
      );

  VkDescriptorType get_vulkan_descriptor_type(
      DescriptorType descriptor_type
      );

}

#endif
