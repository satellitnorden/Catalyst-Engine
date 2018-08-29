#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

/*
*	Turns Vulkan debugging on/off.
*/
#if !defined(CATALYST_FINAL)
	#define VULKAN_DEBUGGING true
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
			PRINT_TO_OUTPUT("Could not find a VkResult case for this result");

			break;
		}

		case VK_ERROR_DEVICE_LOST:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_DEVICE_LOST.");

			break;
		}

		case VK_ERROR_INITIALIZATION_FAILED:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_INITIALIZATION_FAILED.");

			break;
		}

		case VK_ERROR_LAYER_NOT_PRESENT:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_LAYER_NOT_PRESENT.");

			break;
		}

		case VK_ERROR_MEMORY_MAP_FAILED:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_MEMORY_MAP_FAILED.");

			break;
		}

		case VK_ERROR_OUT_OF_DATE_KHR:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_OUT_OF_DATE_KHR.");

			break;
		}

		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_OUT_OF_DEVICE_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_OUT_OF_HOST_MEMORY.");

			break;
		}

		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		{
			PRINT_TO_OUTPUT("Result was VK_ERROR_OUT_OF_POOL_MEMORY_KHR.");

			break;
		}

		case VK_SUCCESS:
		{
			PRINT_TO_OUTPUT("Result was VK_SUCCESS.");

			break;
		}

		case VK_TIMEOUT:
		{
			PRINT_TO_OUTPUT("Result was VK_TIMEOUT.");

			break;
		}
	}

	BREAKPOINT();
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