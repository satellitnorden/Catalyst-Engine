#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Third Party Libraries/vulkan/vulkan.h>

/*
*	Turns RenderDoc debugging on/off.
*/
#if !defined(CATALYST_FINAL)
	#define RENDERDOC_DEBUGGING false
#else
	#define RENDERDOC_DEBUGGING false
#endif

/*
*	Turns Vulkan debugging on/off.
*/
#if !defined(CATALYST_FINAL)
	#define VULKAN_DEBUGGING true
#else
	#define VULKAN_DEBUGGING false
#endif

/*
*	Defines for platform specific Vulkan types.
*/
#if defined(CATALYST_WINDOWS)
	
	//Enumerations.
	#define VULKAN_COLOR_SPACE_SRGB_NONLINEAR VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	#define VULKAN_COMPOSITE_ALPHA_OPAQUE_BIT VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	#define VULKAN_IMAGE_LAYOUT_PRESENT_SRC VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	#define VULKAN_PRESENT_MODE_FIFO VK_PRESENT_MODE_FIFO_KHR
	#define VULKAN_PRESENT_MODE_MAILBOX VK_PRESENT_MODE_MAILBOX_KHR
	#define VULKAN_STRUCTURE_TYPE_PRESENT_INFO VK_STRUCTURE_TYPE_PRESENT_INFO_KHR
	#define VULKAN_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR

	//Functions.
	#define VULKAN_AQUIRE_NEXT_IMAGE vkAcquireNextImageKHR
	#define VULKAN_CREATE_SWAPCHAIN vkCreateSwapchainKHR
	#define VULKAN_DESTROY_SURFACE vkDestroySurfaceKHR
	#define VULKAN_DESTROY_SWAPCHAIN vkDestroySwapchainKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES vkGetPhysicalDeviceSurfaceCapabilitiesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS vkGetPhysicalDeviceSurfaceFormatsKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES vkGetPhysicalDeviceSurfacePresentModesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT vkGetPhysicalDeviceSurfaceSupportKHR
	#define VULKAN_GET_SWAPCHAIN_IMAGES vkGetSwapchainImagesKHR
	#define VULKAN_QUEUUE_PRESENT vkQueuePresentKHR

	//Names.
	#define VULKAN_SWAPCHAIN_EXTENSION_NAME VK_KHR_SWAPCHAIN_EXTENSION_NAME

	//Structures.
	#define VULKAN_PRESENT_INFO_TYPE VkPresentInfoKHR
	#define VULKAN_PRESENT_MODE_TYPE VkPresentModeKHR
	#define VULKAN_SURFACE_TYPE VkSurfaceKHR
	#define VULKAN_SURFACE_CAPABILITIES_TYPE VkSurfaceCapabilitiesKHR
	#define VULKAN_SURFACE_FORMAT_TYPE VkSurfaceFormatKHR
	#define VULKAN_SWAPCHAIN_TYPE VkSwapchainKHR
	#define VULKAN_SWAPCHAIN_CREATE_INFO_TYPE VkSwapchainCreateInfoKHR

#elif defined(CATALYST_ANDROID)

	//Enumerations.
	#define VULKAN_COLOR_SPACE_SRGB_NONLINEAR VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	#define VULKAN_COMPOSITE_ALPHA_OPAQUE_BIT VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	#define VULKAN_IMAGE_LAYOUT_PRESENT_SRC VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	#define VULKAN_PRESENT_MODE_FIFO VK_PRESENT_MODE_FIFO_KHR
	#define VULKAN_PRESENT_MODE_MAILBOX VK_PRESENT_MODE_MAILBOX_KHR
	#define VULKAN_STRUCTURE_TYPE_PRESENT_INFO VK_STRUCTURE_TYPE_PRESENT_INFO_KHR
	#define VULKAN_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR

	//Functions.
	#define VULKAN_AQUIRE_NEXT_IMAGE vkAcquireNextImageKHR
	#define VULKAN_CREATE_SWAPCHAIN vkCreateSwapchainKHR
	#define VULKAN_DESTROY_SURFACE vkDestroySurfaceKHR
	#define VULKAN_DESTROY_SWAPCHAIN vkDestroySwapchainKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES vkGetPhysicalDeviceSurfaceCapabilitiesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS vkGetPhysicalDeviceSurfaceFormatsKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES vkGetPhysicalDeviceSurfacePresentModesKHR
	#define VULKAN_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT vkGetPhysicalDeviceSurfaceSupportKHR
	#define VULKAN_GET_SWAPCHAIN_IMAGES vkGetSwapchainImagesKHR
	#define VULKAN_QUEUUE_PRESENT vkQueuePresentKHR

	//Names.
	#define VULKAN_SWAPCHAIN_EXTENSION_NAME VK_KHR_SWAPCHAIN_EXTENSION_NAME

	//Structures.
	#define VULKAN_PRESENT_INFO_TYPE VkPresentInfoKHR
	#define VULKAN_PRESENT_MODE_TYPE VkPresentModeKHR
	#define VULKAN_SURFACE_TYPE VkSurfaceKHR
	#define VULKAN_SURFACE_CAPABILITIES_TYPE VkSurfaceCapabilitiesKHR
	#define VULKAN_SURFACE_FORMAT_TYPE VkSurfaceFormatKHR
	#define VULKAN_SWAPCHAIN_TYPE VkSwapchainKHR
	#define VULKAN_SWAPCHAIN_CREATE_INFO_TYPE VkSwapchainCreateInfoKHR

#endif

#if VULKAN_DEBUGGING
/*
*	Given a VkResult, return a string.
*/
static void PrintResultAndBreak(const char *const RESTRICT function, const VkResult result) NOEXCEPT
{
	PRINT_TO_CONSOLE(function << " failed.");

	switch (result)
	{
		default:
		{
			PRINT_TO_CONSOLE("Could not find a VkResult case for this result");

			break;
		}

		case VK_ERROR_DEVICE_LOST:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_DEVICE_LOST.");

			break;
		}

		case VK_ERROR_INITIALIZATION_FAILED:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_INITIALIZATION_FAILED.");

			break;
		}

		case VK_ERROR_MEMORY_MAP_FAILED:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_MEMORY_MAP_FAILED.");

			break;
		}

		case VK_ERROR_OUT_OF_DATE_KHR:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_DATE_KHR.");

			break;
		}

		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_DEVICE_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_HOST_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		{
			PRINT_TO_CONSOLE("Result was VK_ERROR_OUT_OF_POOL_MEMORY_KHR.");

			break;
		}

		case VK_SUCCESS:
		{
			PRINT_TO_CONSOLE("Result was VK_SUCCESS.");

			break;
		}

		case VK_TIMEOUT:
		{
			PRINT_TO_CONSOLE("Result was VK_TIMEOUT.");

			break;
		}
	}

	BREAKPOINT;
}
#endif

/*
*	Checks the result of a Vulkan function call and takes appropriate measures if it is not VK_SUCCESS in final.
*/
#if VULKAN_DEBUGGING
	#define VULKAN_ERROR_CHECK(function) { const VkResult result = function; if (result != VK_SUCCESS) PrintResultAndBreak(#function, result); }
#else
	#define VULKAN_ERROR_CHECK(function) function
#endif