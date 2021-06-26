#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Third party.
#include <ThirdParty/VrApi/VrApi.h>
#include <ThirdParty/VrApi/VrApi_Vulkan.h>

/*
*	Returns the required instance extensions.
*/
void VulkanPlatform::RequiredInstanceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT
{
    //Retrieve the instance extensions names.
    static char instance_extension_names[4096];
    uint32 instance_extension_names_size{ sizeof(instance_extension_names) };

    if (vrapi_GetInstanceExtensionsVulkan(instance_extension_names, &instance_extension_names_size))
    {
        ASSERT(false, "oh no");
    }

    else
    {
        output->Emplace(instance_extension_names);

        for (uint32 i{ 0 }; i < instance_extension_names_size; ++i)
        {
            if (instance_extension_names[i] == ' ')
            {
                instance_extension_names[i] = '\0';
                output->Emplace(&instance_extension_names[i + 1]);
            }
        }

        instance_extension_names[instance_extension_names_size] = '\0';
    }
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
