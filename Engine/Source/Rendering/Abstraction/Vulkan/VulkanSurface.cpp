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
	PlatformVulkan::CreateVulkanSurface(&_VulkanSurface);
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