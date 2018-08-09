//Header file.
#include <Rendering/API/Vulkan/VulkanSurface.h>

//Graphics.
#include <Rendering/Engine/Window.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

#if defined(CATALYST_WINDOWS)
//Third party libraries.
#include <Third Party Libraries/GLFW/glfw3.h>
#endif

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
#if defined(CATALYST_WINDOWS)
	//Create the Vulkan surface!
	VULKAN_ERROR_CHECK(glfwCreateWindowSurface(VulkanInterface::Instance->GetInstance().Get(), window.Get(), nullptr, &vulkanSurface));
#endif
}

/*
*	Releases this Vulkan surface.
*/
void VulkanSurface::Release() NOEXCEPT
{
	//Destroy the Vulkan surface.
	VULKAN_DESTROY_SURFACE(VulkanInterface::Instance->GetInstance().Get(), vulkanSurface, nullptr);
}