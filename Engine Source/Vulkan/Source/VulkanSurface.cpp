//Header file.
#include <VulkanSurface.h>

//Graphics.
#include <Window.h>

//Vulkan.
#include <VulkanInstance.h>

//Third party libraries.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/*
*	Default constructor.
*/
VulkanSurface::VulkanSurface() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSurface::~VulkanSurface() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan surface.
*/
void VulkanSurface::Initialize(const VulkanInstance &vulkanInstance, Window &window) CATALYST_NOEXCEPT
{
	//Create the Vulkan surface!
	VkResult result = glfwCreateWindowSurface(vulkanInstance.Get(), window.Get(), nullptr, &vulkanSurface);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release(const VulkanInstance &vulkanInstance) CATALYST_NOEXCEPT
{
	//Destroy the Vulkan surface.
	VULKAN_DESTROY_SURFACE(vulkanInstance.Get(), vulkanSurface, nullptr);
}