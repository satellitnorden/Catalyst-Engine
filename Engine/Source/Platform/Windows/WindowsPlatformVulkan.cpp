#if defined(CATALYST_WINDOWS)
//Header file.
#include <Platform/Windows/WindowsPlatformVulkan.h>

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
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;

		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = nullptr;
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.hinstance = CatalystPlatform::_Instance;
		surfaceCreateInfo.hwnd = CatalystPlatform::_Window;

		const VkResult result{ vkCreateWin32SurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surfaceCreateInfo, nullptr, vulkanSurface) };

		ASSERT(result == VK_SUCCESS, "Could not create Vulkan surface!");
	}

}
#endif