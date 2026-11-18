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
	VkAndroidSurfaceCreateInfoKHR surface_create_info;

	surface_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surface_create_info.pNext = nullptr;
	surface_create_info.flags = 0;
	surface_create_info.window = CatalystPlatform::_Window;

	VULKAN_ERROR_CHECK(vkCreateAndroidSurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surface_create_info, nullptr, &_VulkanSurface));
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	VkWin32SurfaceCreateInfoKHR surface_create_info;

	surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_create_info.pNext = nullptr;
	surface_create_info.flags = 0;
	surface_create_info.hinstance = CatalystPlatformWindows::_Instance;
	surface_create_info.hwnd = static_cast<HWND>(Platform::WindowManager::GetWindowHandle(Platform::WindowManager::GetMasterWindow()));

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surface_create_info, nullptr, &_VulkanSurface));
#endif

#if defined(CATALYST_PLATFORM_LINUX)
	VkWaylandSurfaceCreateInfoKHR surface_create_info;

	surface_create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surface_create_info.pNext = nullptr;
	surface_create_info.flags = 0;
	surface_create_info.display = static_cast<wl_display*>(Platform::WindowManager::GetDisplayHandle(Platform::WindowManager::GetMasterWindow()));
	surface_create_info.surface = static_cast<wl_surface*>(Platform::WindowManager::GetSurfaceHandle(Platform::WindowManager::GetMasterWindow()));

	VULKAN_ERROR_CHECK(vkCreateWaylandSurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), &surface_create_info, nullptr, &_VulkanSurface));
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