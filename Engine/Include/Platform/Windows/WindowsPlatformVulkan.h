#if defined(CATALYST_WINDOWS)

#pragma once

//Core.
#include <Core/EngineCore.h>

//Third party.
#include <ThirdParty/vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/GLFW/glfw3.h>

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

}

#endif