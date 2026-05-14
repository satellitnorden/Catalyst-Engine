#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanInstance.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

//System.
#include <Systems/CatalystEngineSystem.h>

#if VULKAN_DEBUGGING
//Define the validation layers.
namespace
{
#define CRASH_DIAGNOSTIC (0)

#if CRASH_DIAGNOSTIC
	constexpr StaticArray<const char *const RESTRICT, 2> WANTED_VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_crash_diagnostic" };
#else
	constexpr StaticArray<const char *const RESTRICT, 1> WANTED_VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation" };
#endif
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
	VkApplicationInfo application_info;
	
	{
		PROFILING_SCOPE("Create Application Info");
		CreateApplicationInfo(application_info);
	}

	//Create the instance create info.
	VkInstanceCreateInfo instance_create_info;
	
	{
		PROFILING_SCOPE("Create Instance Create Info");
		CreateInstanceCreateInfo(instance_create_info, application_info);
	}

	//Create the instance!
	{
		PROFILING_SCOPE("vkCreateInstance");
		VULKAN_ERROR_CHECK(vkCreateInstance(&instance_create_info, nullptr, &_VulkanInstance));
	}
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
	applicationInfo.apiVersion = VK_API_VERSION_1_4;
#endif
}

/*
*	Creates the instance create info.
*/
void VulkanInstance::CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const NOEXCEPT
{
#if VULKAN_DEBUGGING
	static VkValidationFeaturesEXT VALIDATION_FEATURES_EXTENSION;
#endif

	createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if VULKAN_DEBUGGING
	createInstanceInfo.pNext = &VALIDATION_FEATURES_EXTENSION;
#else
	createInstanceInfo.pNext = nullptr;
#endif
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

	//Get the required instance extensions for the platform.
	VulkanPlatform::RequiredInstanceExtensions(&extensions);

	createInstanceInfo.enabledExtensionCount = static_cast<uint32>(extensions.Size());
	createInstanceInfo.ppEnabledExtensionNames = (const char *const *) extensions.Data();

#if VULKAN_DEBUGGING
	static VkValidationFeatureEnableEXT VALIDATION_FEATURES_ENABLED[]
	{
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
		VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
	};

	VALIDATION_FEATURES_EXTENSION = { };
	VALIDATION_FEATURES_EXTENSION.sType = VkStructureType::VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	VALIDATION_FEATURES_EXTENSION.pNext = nullptr;
	VALIDATION_FEATURES_EXTENSION.enabledValidationFeatureCount = ARRAY_LENGTH(VALIDATION_FEATURES_ENABLED);
	VALIDATION_FEATURES_EXTENSION.pEnabledValidationFeatures = VALIDATION_FEATURES_ENABLED;
#endif
}
#endif