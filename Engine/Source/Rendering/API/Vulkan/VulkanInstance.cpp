//Header file.
#include <Rendering/API/Vulkan/VulkanInstance.h>

//Systems.
#include <Systems/EngineSystem.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

#if VULKAN_DEBUGGING && !defined(CATALYST_ANDROID)
//Define the validation layers.
namespace
{
	constexpr StaticArray<const char *const RESTRICT, 2> validationLayers{ "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_assistant_layer" };
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
	VULKAN_ERROR_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance));
}

/*
*	Releases this Vulkan instance.
*/
void VulkanInstance::Release() NOEXCEPT
{
	//Destroy the Vulkan instance.
	vkDestroyInstance(vulkanInstance, nullptr);
}

/*
*	Creates the application info.
*/
void VulkanInstance::CreateApplicationInfo(VkApplicationInfo &applicationInfo) const NOEXCEPT
{
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = EngineSystem::Instance->GetProjectConfiguration().generalConfiguration.projectName.CString();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Catalyst Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VULKAN_API_VERSION;
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

#if VULKAN_DEBUGGING && !defined(CATALYST_ANDROID)
	createInstanceInfo.enabledLayerCount = static_cast<uint32>(validationLayers.Size());
	createInstanceInfo.ppEnabledLayerNames = validationLayers.Data();
#else
	createInstanceInfo.enabledLayerCount = 0;
	createInstanceInfo.ppEnabledLayerNames = nullptr;
#endif

	PlatformVulkan::GetRequiredInstanceExtensions(extensions);

	createInstanceInfo.enabledExtensionCount = static_cast<uint32>(extensions.Size());
	createInstanceInfo.ppEnabledExtensionNames = (const char *const *) extensions.Data();
}