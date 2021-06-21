#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanInstance.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//System.
#include <Systems/CatalystEngineSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

#if VULKAN_DEBUGGING
//Define the validation layers.
namespace
{
	constexpr StaticArray<const char *const RESTRICT, 1> WANTED_VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation" };
	DynamicArray<const char *const RESTRICT> ENABLED_VALIDATION_LAYERS;
}
#endif

//The instance extensions.
DynamicArray<const char *const RESTRICT> extensions;

/*
*	Initializes this Vulkan instance.
*/
void VulkanInstance::Initialize() NOEXCEPT
{
	//Create the application info.
	VkApplicationInfo applicationInfo;
	CreateApplicationInfo(applicationInfo);

	//Create the instance create info.
	VkInstanceCreateInfo instanceCreateInfo;
	CreateInstanceCreateInfo(instanceCreateInfo, applicationInfo);

	//Create the instance!
	VULKAN_ERROR_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &_VulkanInstance));
}

/*
*	Releases this Vulkan instance.
*/
void VulkanInstance::Release() NOEXCEPT
{
	//Destroy the Vulkan instance.
	vkDestroyInstance(_VulkanInstance, nullptr);
}

/*
*	Creates the application info.
*/
void VulkanInstance::CreateApplicationInfo(VkApplicationInfo &applicationInfo) const NOEXCEPT
{
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = CATALYST_ENGINE_NAME;
	applicationInfo.engineVersion = VK_MAKE_VERSION(CATALYST_ENGINE_MAJOR_VERSION, CATALYST_ENGINE_MINOR_VERSION, CATALYST_ENGINE_PATCH_VERSION);

#if defined(CATALYST_PLATFORM_ANDROID)
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	applicationInfo.apiVersion = VK_API_VERSION_1_1;
#endif
}

/*
*	Creates the instance create info.
*/
void VulkanInstance::CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const NOEXCEPT
{
	createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInstanceInfo.pNext = nullptr;
	createInstanceInfo.flags = 0;
	createInstanceInfo.pApplicationInfo = &applicationInfo;

#if VULKAN_DEBUGGING
	uint32 instance_layer_present_count{ 0 };
	vkEnumerateInstanceLayerProperties(&instance_layer_present_count, nullptr);

	if (instance_layer_present_count > 0)
	{
		DynamicArray<VkLayerProperties> layer_properties;
		layer_properties.Upsize<false>(instance_layer_present_count);

		vkEnumerateInstanceLayerProperties(&instance_layer_present_count, layer_properties.Data());

		for (const VkLayerProperties &layer_property : layer_properties)
		{
			for (const char *const RESTRICT wanted_validation_layer : WANTED_VALIDATION_LAYERS)
			{
				if (StringUtilities::IsEqual(layer_property.layerName, wanted_validation_layer))
				{
					ENABLED_VALIDATION_LAYERS.Emplace(wanted_validation_layer);

					break;
				}
			}
		}

		createInstanceInfo.enabledLayerCount = static_cast<uint32>(ENABLED_VALIDATION_LAYERS.Size());
		createInstanceInfo.ppEnabledLayerNames = ENABLED_VALIDATION_LAYERS.Data();
	}

	else
#endif
	{
		createInstanceInfo.enabledLayerCount = 0;
		createInstanceInfo.ppEnabledLayerNames = nullptr;
	}

#if VULKAN_DEBUGGING && !defined(CATALYST_PLATFORM_ANDROID)
	extensions.Reserve(3);
#else
	extensions.Reserve(2);
#endif

	extensions.Emplace(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(CATALYST_PLATFORM_ANDROID)
	extensions.Emplace(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	extensions.Emplace(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#if VULKAN_DEBUGGING && !defined(CATALYST_PLATFORM_ANDROID)
	extensions.Emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	createInstanceInfo.enabledExtensionCount = static_cast<uint32>(extensions.Size());
	createInstanceInfo.ppEnabledExtensionNames = (const char *const *) extensions.Data();
}
#endif