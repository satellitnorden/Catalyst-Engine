#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Platform.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/VulkanCoreWindows.h>
#elif defined(CATALYST_PLATFORM_ANDROID)
	#include <Platform/Android/VulkanCoreAndroid.h>
#endif

/*
*	Turns Vulkan debugging on/off.
*/
#if defined(CATALYST_CONFIGURATION_DEBUG)
	#define VULKAN_DEBUGGING false
#else
	#define VULKAN_DEBUGGING false
#endif

#if VULKAN_DEBUGGING
/*
*	Given a VkResult, return a string.
*/
static void PrintResultAndBreak(const char *const RESTRICT function, const VkResult result) NOEXCEPT
{
	PRINT_TO_OUTPUT(function << " failed.");

	switch (result)
	{
		default:
		{
			ASSERT(false, "Could not find a VkResult case for this result");

			break;
		}

		case VK_ERROR_DEVICE_LOST:
		{
			ASSERT(false, "Result was VK_ERROR_DEVICE_LOST.");

			break;
		}

		case VK_ERROR_EXTENSION_NOT_PRESENT:
		{
			ASSERT(false, "Result was VK_ERROR_EXTENSION_NOT_PRESENT.");

			break;
		}

		case VK_ERROR_FEATURE_NOT_PRESENT:
		{
			ASSERT(false, "Result was VK_ERROR_FEATURE_NOT_PRESENT.");

			break;
		}

		case VK_ERROR_INITIALIZATION_FAILED:
		{
			ASSERT(false, "Result was VK_ERROR_INITIALIZATION_FAILED.");

			break;
		}

		case VK_ERROR_LAYER_NOT_PRESENT:
		{
			ASSERT(false, "Result was VK_ERROR_LAYER_NOT_PRESENT.");

			break;
		}

		case VK_ERROR_MEMORY_MAP_FAILED:
		{
			ASSERT(false, "Result was VK_ERROR_MEMORY_MAP_FAILED.");

			break;
		}

		case VK_ERROR_OUT_OF_DATE_KHR:
		{
			ASSERT(false, "Result was VK_ERROR_OUT_OF_DATE_KHR.");

			break;
		}

		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		{
			ASSERT(false, "Result was VK_ERROR_OUT_OF_DEVICE_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			ASSERT(false, "Result was VK_ERROR_OUT_OF_HOST_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		{
			ASSERT(false, "Result was VK_ERROR_OUT_OF_POOL_MEMORY_KHR.");

			break;
		}

		case VK_ERROR_TOO_MANY_OBJECTS:
		{
			ASSERT(false, "Result was VK_ERROR_TOO_MANY_OBJECTS.");

			break;
		}

		case VK_SUCCESS:
		{
			ASSERT(false, "Result was VK_SUCCESS.");

			break;
		}

		case VK_TIMEOUT:
		{
			ASSERT(false, "Result was VK_TIMEOUT.");

			break;
		}
	}
}
#endif

/*
*	Checks the result of a Vulkan function call and takes appropriate measures if it fails.
*/
#if VULKAN_DEBUGGING
	#define VULKAN_ERROR_CHECK(function) { const VkResult result = function; if (result != VK_SUCCESS) PrintResultAndBreak(#function, result); }
#else
	#define VULKAN_ERROR_CHECK(function) function
#endif
#endif