#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

/*
*	Returns the required instance extensions.
*/
void VulkanPlatform::RequiredInstanceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT
{
	output->Emplace(VK_KHR_SURFACE_EXTENSION_NAME);
	output->Emplace(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#if VULKAN_DEBUGGING
	output->Emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

/*
*	Returns the required logical device extensions.
*/
void VulkanPlatform::RequiredLogicalDeviceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT
{
	output->Emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

/*
*	Callback for when the logical device has been created.
*/
void VulkanPlatform::OnLogicalDeviceCreated() NOEXCEPT
{

}

/*
*	Initializes the swapchain.
*/
void VulkanPlatform::InitializeSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT
{

}

/*
*	Callback for when a frame is started.
*/
void VulkanPlatform::BeginFrame() NOEXCEPT
{

}

/*
*	Releases the swapchain.
*/
void VulkanPlatform::ReleaseSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT
{

}

/*
*	Terminates Vulkan for the current platform.
*/
void VulkanPlatform::Terminate() NOEXCEPT
{

}
#endif
