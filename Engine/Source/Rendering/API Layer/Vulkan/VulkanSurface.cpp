//Header file.
#include <Rendering/API Layer/Vulkan/VulkanSurface.h>

//Graphics.
#include <Rendering/Engine Layer/Window.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

//Third party libraries.
#include <Third Party Libraries/GLFW/glfw3.h>

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
void VulkanSurface::Initialize(Window &window) NOEXCEPT
{
	//Create the Vulkan surface!
	VULKAN_ERROR_CHECK(glfwCreateWindowSurface(VulkanInterface::Instance->GetInstance().Get(), window.Get(), nullptr, &vulkanSurface));
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() NOEXCEPT
{
	//Destroy the Vulkan surface.
	VULKAN_DESTROY_SURFACE(VulkanInterface::Instance->GetInstance().Get(), vulkanSurface, nullptr);
}