#if defined(CATALYST_WINDOWS)
//Header file.
#include <Platform/Windows/WindowsPlatformVulkan.h>

//Rendering.
#include <Rendering/Engine/Surface.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

namespace PlatformVulkan
{

	/*
	*	Creates the Vulkan surface.
	*/
	void CreateVulkanSurface(VkSurfaceKHR *const RESTRICT vulkanSurface) NOEXCEPT
	{
		const VkResult result{ glfwCreateWindowSurface(VulkanInterface::Instance->GetInstance().Get(), static_cast<GLFWwindow *const RESTRICT>(RenderingSystem::Instance->GetMainSurface()->Get()), nullptr, vulkanSurface) };

		ASSERT(result == VK_SUCCESS, "Could not create Vulkan surface!");
	}

}
#endif