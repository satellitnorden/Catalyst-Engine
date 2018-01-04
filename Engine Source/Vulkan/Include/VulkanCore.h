#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <vulkan/vulkan.h>

/*
*	Alias types for platform specific Vulkantypes.
*/
#if defined(CATALYST_WINDOWS)
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES vkGetPhysicalDeviceSurfaceCapabilitiesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS vkGetPhysicalDeviceSurfaceFormatsKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES vkGetPhysicalDeviceSurfacePresentModesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT vkGetPhysicalDeviceSurfaceSupportKHR

	#define VULKAN_PRESENT_INFO_TYPE VkPresentInfoKHR
	#define VULKAN_PRESENT_MODE_TYPE VkPresentModeKHR
	#define VULKAN_SURFACE_TYPE VkSurfaceKHR
	#define VULKAN_SURFACE_CAPABILITIES_TYPE VkSurfaceCapabilitiesKHR
	#define VULKAN_SURFACE_FORMAT_TYPE VkSurfaceFormatKHR
	#define VULKAN_SWAPCHAIN_TYPE VkSwapchainKHR
	#define VULKAN_SWAPCHAIN_CREATE_INFO_TYPE VkSwapchainCreateInfoKHR

	#define VULKAN_SWAPCHAIN_EXTENSION_NAME VK_KHR_SWAPCHAIN_EXTENSION_NAME
#endif

/*
*	Checks the result of a Vulkan function call and takes appropriate measures if it is not VK_SUCCESS in final.
*/
#if !defined(CATALYST_FINAL)
	#define VULKAN_ERROR_CHECK(function) { if ((function) != VKSUCCESS) BREAKPOINT; }
#else
	#define VULKAN_ERROR_CHECK(function) function
#endif