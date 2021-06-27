#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

//Third party.
#include <ThirdParty/VrApi/VrApi.h>
#include <ThirdParty/VrApi/VrApi_Helpers.h>
#include <ThirdParty/VrApi/VrApi_Vulkan.h>

//Vulkan platform oculus quest data.
namespace VulkanPlatformOculusQuestData
{

    //The swapchain.
    ovrTextureSwapChain *RESTRICT _Swapchain;

}

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
    //Retrieve the logical device extensions names.
    static char logical_device_extension_names[4096];
    uint32 logical_device_extension_names_size{ sizeof(logical_device_extension_names) };

    if (vrapi_GetDeviceExtensionsVulkan(logical_device_extension_names, &logical_device_extension_names_size))
    {
        ASSERT(false, "oh no");
    }

    else
    {
        output->Emplace(logical_device_extension_names);

        for (uint32 i{ 0 }; i < logical_device_extension_names_size; ++i)
        {
            if (logical_device_extension_names[i] == ' ')
            {
                logical_device_extension_names[i] = '\0';
                output->Emplace(&logical_device_extension_names[i + 1]);
            }
        }

        logical_device_extension_names[logical_device_extension_names_size] = '\0';
    }
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

	if (vrapi_CreateSystemVulkan(&system_create_info_vulkan))
    {
	    ASSERT(false, "oh no");
    }
}

/*
*	Initializes the swapchain.
*/
void VulkanPlatform::InitializeSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT
{
    //Retrieve the resolution.
    const Resolution resolution{ CatalystPlatform::GetDefaultResolution() };

    //Create the swapchain.
    VulkanPlatformOculusQuestData::_Swapchain = vrapi_CreateTextureSwapChain3( VRAPI_TEXTURE_TYPE_2D_ARRAY,
                                                                               VK_FORMAT_R8G8B8A8_UNORM,
                                                                               resolution._Width,
                                                                               resolution._Height,
                                                                               1,
                                                                               3);

    //Retrieve the swapchain length.
    const uint32 swapchain_length{ static_cast<uint32>(vrapi_GetTextureSwapChainLength(VulkanPlatformOculusQuestData::_Swapchain)) };

    //Set the number of swapchain images.
    swapchain->SetNumberOfSwapchainImages(swapchain_length);

    //Retrieve all the swapchain images.
    for (uint32 i{ 0 }; i < swapchain_length; ++i)
    {
        swapchain->AddSwapchainImage(vrapi_GetTextureSwapChainBufferVulkan(VulkanPlatformOculusQuestData::_Swapchain, i));
    }

    //Add all the swapchain image view.
    for (uint32 i{ 0 }; i < swapchain_length; ++i)
    {
        VkImageView new_image_view;
        VulkanUtilities::CreateVulkanImageView(swapchain->GetSwapchainImages()[i], VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1, 2, new_image_view);
        swapchain->AddSwapchainImageView(new_image_view);
    }

    //Set the swap extent.
    swapchain->SetSwapExtent(VkExtent2D{ resolution._Width, resolution._Height });
}

/*
*	Callback for when a frame is started.
*/
void VulkanPlatform::BeginFrame() NOEXCEPT
{
    //Should the application enter vr mode?
    const bool should_enter_vr_mode{ CatalystPlatform::_NativeWindow != nullptr && CatalystPlatform::_IsResumed };

    if (should_enter_vr_mode && !CatalystPlatform::_ovrMobile)
    {
        ovrModeParmsVulkan mode_parameters_vulkan{ vrapi_DefaultModeParmsVulkan(&CatalystPlatform::_ovrJava, (uint64)VulkanInterface::Instance->GetGraphicsQueue()->Get()) };

        mode_parameters_vulkan.ModeParms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
        mode_parameters_vulkan.ModeParms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
        mode_parameters_vulkan.ModeParms.WindowSurface = (size_t)CatalystPlatform::_NativeWindow;
        mode_parameters_vulkan.ModeParms.Display = 0;
        mode_parameters_vulkan.ModeParms.ShareContext = 0;

        CatalystPlatform::_ovrMobile = vrapi_EnterVrMode((ovrModeParms*)&mode_parameters_vulkan);

        //If entering VR mode failed then the ANativeWindow was not valid.
        if (!CatalystPlatform::_ovrMobile)
        {
            CatalystPlatform::_NativeWindow = nullptr;
        }

        //Set performance parameters once we have entered VR mode and have a valid ovrMobile.
        if (CatalystPlatform::_ovrMobile)
        {
            vrapi_SetClockLevels(CatalystPlatform::_ovrMobile, 1, 1);
            //vrapi_SetPerfThread(CatalystPlatform::_ovrMobile, VRAPI_PERF_THREAD_TYPE_MAIN, app->MainThreadTid);
            //vrapi_SetPerfThread(CatalystPlatform::_ovrMobile, VRAPI_PERF_THREAD_TYPE_RENDERER, app->RenderThreadTid);
        }
    }

    else if (!should_enter_vr_mode && CatalystPlatform::_ovrMobile)
    {
        vrapi_LeaveVrMode(CatalystPlatform::_ovrMobile);
        CatalystPlatform::_ovrMobile = nullptr;
    }
}

/*
*	Releases the swapchain.
*/
void VulkanPlatform::ReleaseSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT
{
    //Destroy all image views.
    for (auto &swapChainImageView : swapchain->GetSwapchainImageViews())
    {
        vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), swapChainImageView, nullptr);
    }

    //Destroy the swapchain.
    vrapi_DestroyTextureSwapChain(VulkanPlatformOculusQuestData::_Swapchain);
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
