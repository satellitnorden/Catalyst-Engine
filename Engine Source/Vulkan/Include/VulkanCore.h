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
*	Given a VkResult, return a string.
*/
static void PrintResultAndBreak(const VkResult result) CATALYST_NOEXCEPT
{
	switch (result)
	{
		default:
		{
			PRINT_TO_CONSOLE("Could not find a VkResult case for this result");
			BREAKPOINT;

			break;
		}

		case VK_ERROR_DEVICE_LOST:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_DEVICE_LOST.");
			BREAKPOINT;

			break;
		}

		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_DEVICE_MEMORY.");
			BREAKPOINT;

			break;
		}

		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_HOST_MEMORY.");
			BREAKPOINT;

			break;
		}

		case VK_SUCCESS:
		{
			PRINT_TO_CONSOLE("Result was VK_SUCCESS.");
			BREAKPOINT;

			break;
		}

		case VK_TIMEOUT:
		{
			PRINT_TO_CONSOLE("Result was VK_TIMEOUT.");
			BREAKPOINT;

			break;
		}
	}
}

/*
*	Checks the result of a Vulkan function call and takes appropriate measures if it is not VK_SUCCESS in final.
*/
#if !defined(CATALYST_FINAL)
	#define VULKAN_ERROR_CHECK(function) { const VkResult result = function; if (result != VK_SUCCESS) PrintResultAndBreak(result); }
#else
	#define VULKAN_ERROR_CHECK(function) function
#endif