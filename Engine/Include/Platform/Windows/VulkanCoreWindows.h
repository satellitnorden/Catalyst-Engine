#if defined(CATALYST_RENDERING_VULKAN)
#if defined(CATALYST_PLATFORM_WINDOWS)

#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Third party.
#define VK_USE_PLATFORM_WIN32_KHR
#include <ThirdParty/vulkan/vulkan.h>

/*
*	Defines the Vulkan API version.
*/
#define VULKAN_API_VERSION VK_API_VERSION_1_1

namespace PlatformVulkan
{

	/*
	*	Loads Vulkan.
	*/
	static void LoadVulkan() NOEXCEPT
	{
		//Nothing to do here on Windows.
	}

	/*
	*	Returns the required Vulkan instance extensions.
	*/
	static void GetRequiredInstanceExtensions(DynamicArray<const char *const RESTRICT> &extensions) NOEXCEPT
	{
		extensions.Reserve(2);

		extensions.EmplaceFast(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.EmplaceFast(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	}

	/*
	*	Creates the Vulkan surface.
	*/
	void CreateVulkanSurface(VkSurfaceKHR *const RESTRICT vulkanSurface) NOEXCEPT;

}

#endif
#endif