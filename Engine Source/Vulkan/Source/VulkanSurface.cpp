//Header file.
#include <VulkanSurface.h>

//Graphics.
#include <Window.h>

//Vulkan.
#include <VulkanInterface.h>

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
void VulkanSurface::Initialize(Window &window) CATALYST_NOEXCEPT
{
	//Create the Vulkan surface!
	VULKAN_ERROR_CHECK(glfwCreateWindowSurface(VulkanInterface::Instance->GetInstance().Get(), window.Get(), nullptr, &vulkanSurface));
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan surface.
	VULKAN_DESTROY_SURFACE(VulkanInterface::Instance->GetInstance().Get(), vulkanSurface, nullptr);
}