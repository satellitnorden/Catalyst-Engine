#if defined(CATALYST_HAS_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSurface.h>

//Platform.
#include <Platform/Core/WindowManager.h>
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>
#endif

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan surface.
*/
void VulkanSurface::Initialize() NOEXCEPT
{
	if (_VulkanSurface != VK_NULL_HANDLE)
	{
		return;
	}

#if defined(CATALYST_PLATFORM_ANDROID) || defined(CATALYST_PLATFORM_OCULUS_QUEST)
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
	surfaceCreateInfo.hinstance = CatalystPlatformWindows::_Instance;
	surfaceCreateInfo.hwnd = static_cast<HWND>(Platform::WindowManager::GetDisplayHandle(Platform::WindowManager::GetMasterWindow()));

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surfaceCreateInfo, nullptr, &_VulkanSurface));
#endif

#if defined(CATALYST_PLATFORM_LINUX)
	VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo;

	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.display = static_cast<wl_display*>(Platform::WindowManager::GetDisplayHandle(Platform::WindowManager::GetMasterWindow()));
	surfaceCreateInfo.surface = static_cast<wl_surface*>(Platform::WindowManager::GetSurfaceHandle(Platform::WindowManager::GetMasterWindow()));

	VULKAN_ERROR_CHECK(vkCreateWaylandSurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surfaceCreateInfo, nullptr, &_VulkanSurface));
#endif
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() NOEXCEPT
{
	if (_VulkanSurface == VK_NULL_HANDLE)
	{
		return;
	}

	//Destroy the Vulkan surface.
	vkDestroySurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), _VulkanSurface, nullptr);

	//Reset the Vulkan surface.
	_VulkanSurface = VK_NULL_HANDLE;
}
#endif