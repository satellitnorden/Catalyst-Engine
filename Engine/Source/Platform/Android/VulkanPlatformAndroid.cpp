#if defined(CATALYST_PLATFORM_ANDROID)
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
	output->Emplace(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
}

/*
*	Returns the required logical device extensions.
*/
void VulkanPlatform::RequiredLogicalDeviceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT
{

}

/*
*	Callback for when the logical device has been created.
*/
void VulkanPlatform::OnLogicalDeviceCreated() NOEXCEPT
{

}

/*
*	Terminates Vulkan for the current platform.
*/
void VulkanPlatform::Terminate() NOEXCEPT
{

}
#endif
