#if defined(CATALYST_WINDOWS)

#pragma once

//Core.
#include <Core/EngineCore.h>

//Third party.
#include <ThirdParty/vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/GLFW/glfw3.h>

/*
*	Defines the Vulkan API version.
*/
#define VULKAN_API_VERSION VK_API_VERSION_1_1

//Forward declarations.
class Surface;

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
	static void GetRequiredInstanceExtensions(DynamicArray<const char *RESTRICT> &extensions) NOEXCEPT
	{
		uint32 glfwExtensionCount{ 0 };
		const char *RESTRICT *RESTRICT glfwExtensions{ nullptr };

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		extensions.Reserve(glfwExtensionCount);

		for (uint32 i = 0; i < glfwExtensionCount; ++i)
		{
			extensions.EmplaceFast(glfwExtensions[i]);
		}
	}

	/*
	*	Creates the Vulkan surface.
	*/
	void CreateVulkanSurface(VkSurfaceKHR *const RESTRICT vulkanSurface) NOEXCEPT;

}

#endif