#pragma once

#include <vulkan/vulkan.h>

/*
To create a debug messenger, we need to use the vkCreateDebugUtilsMessengerEXT function. However, this is not automatically
loaded, so we must create our own function!
*/

extern VkResult createDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger);

extern void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
