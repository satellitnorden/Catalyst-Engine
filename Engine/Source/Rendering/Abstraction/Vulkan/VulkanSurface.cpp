#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSurface.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan surface.
*/
void VulkanSurface::Initialize() NOEXCEPT
{
#if defined(CATALYST_PLATFORM_ANDROID)
	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo;

	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.window = CatalystPlatform::_Window;

	VULKAN_ERROR_CHECK(vkCreateAndroidSurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surfaceCreateInfo, nullptr, &_VulkanSurface));
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;

	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = CatalystPlatform::_Instance;
	surfaceCreateInfo.hwnd = CatalystPlatform::_Window;

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surfaceCreateInfo, nullptr, &_VulkanSurface));
#endif
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() NOEXCEPT
{
	//Destroy the Vulkan surface.
	vkDestroySurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), _VulkanSurface, nullptr);
}
#endif