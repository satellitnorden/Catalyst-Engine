//Header file.
#include <Rendering/API/Vulkan/VulkanSurface.h>

//Graphics.
#include <Rendering/Engine/Surface.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanSurface::VulkanSurface() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSurface::~VulkanSurface() NOEXCEPT
{

}

/*
*	Initializes this Vulkan surface.
*/
void VulkanSurface::Initialize(Surface &surface) NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Create the Vulkan surface!
	VULKAN_ERROR_CHECK(glfwCreateWindowSurface(VulkanInterface::Instance->GetInstance().Get(), surface.Get(), nullptr, &vulkanSurface));
#endif
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() NOEXCEPT
{
	//Destroy the Vulkan surface.
	vkDestroySurfaceKHR(VulkanInterface::Instance->GetInstance().Get(), vulkanSurface, nullptr);
}