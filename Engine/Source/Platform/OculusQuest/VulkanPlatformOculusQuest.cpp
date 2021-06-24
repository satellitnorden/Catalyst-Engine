#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Third party.
#include <ThirdParty/VrApi/VrApi.h>
#include <ThirdParty/VrApi/VrApi_Vulkan.h>

/*
*	Callback for when the logical device has been created.
*/
void VulkanPlatform::OnLogicalDeviceCreated() NOEXCEPT
{
	
	//Tell VrAPi to create the Vulkan system.
	ovrSystemCreateInfoVulkan system_create_info_vulkan;

	system_create_info_vulkan.Instance = VulkanInterface::Instance->GetInstance().Get();
	system_create_info_vulkan.PhysicalDevice = VulkanInterface::Instance->GetPhysicalDevice().Get();
	system_create_info_vulkan.Device = VulkanInterface::Instance->GetLogicalDevice().Get();

	vrapi_CreateSystemVulkan(&system_create_info_vulkan);
	
}

/*
*	Terminates Vulkan for the current platform.
*/
void VulkanPlatform::Terminate() NOEXCEPT
{
	//Tell VrApi to destroy the Vulkan system.
	vrapi_DestroySystemVulkan();
}
#endif
