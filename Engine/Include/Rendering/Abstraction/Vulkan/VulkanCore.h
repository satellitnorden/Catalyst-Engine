#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//ThirdParty
#define VK_USE_PLATFORM_WIN32_KHR
#include <ThirdParty/vulkan/vulkan.h>

/*
*	Turns Vulkan debugging on/off.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define VULKAN_DEBUGGING (1)
#else
	#define VULKAN_DEBUGGING (0)
#endif

/*
*	Turns Vulkan timestamps on/off.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define VULKAN_TIMESTAMPS_ENABLED (0)
#else
	#define VULKAN_TIMESTAMPS_ENABLED (0)
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