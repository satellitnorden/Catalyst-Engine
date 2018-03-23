//Header file.
#include <Rendering/API Layer/Vulkan/VulkanInstance.h>

//Systems.
#include <Systems/EngineSystem.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Third party libraries.
#include <Third Party Libraries/GLFW/glfw3.h>

#if VULKAN_DEBUGGING
//Define the validation layers.
namespace
{
	const DynamicArray<const char *RESTRICT> validationLayers{ "VK_LAYER_LUNARG_standard_validation" };
}
#endif

/*
*	Default constructor.
*/
VulkanInstance::VulkanInstance() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanInstance::~VulkanInstance() NOEXCEPT
{

}

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
	applicationInfo.pApplicationName = EngineSystem::Instance->GetProjectInformation().generalInformation.projectName.CString();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Catalyst Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
}

/*
*	Creates the instance create info.
*/
void VulkanInstance::CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const NOEXCEPT
{
	uint32 glfwExtensionCount{ 0 };
	const char *RESTRICT *RESTRICT glfwExtensions{ nullptr };

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInstanceInfo.pNext = nullptr;
	createInstanceInfo.flags = 0;
	createInstanceInfo.pApplicationInfo = &applicationInfo;

#if VULKAN_DEBUGGING
	createInstanceInfo.enabledLayerCount = StaticCast<uint32>(validationLayers.Size());
	createInstanceInfo.ppEnabledLayerNames = validationLayers.Data();
#else
	createInstanceInfo.enabledLayerCount = 0;
	createInstanceInfo.ppEnabledLayerNames = nullptr;
#endif

	createInstanceInfo.enabledExtensionCount = glfwExtensionCount;
	createInstanceInfo.ppEnabledExtensionNames = glfwExtensions;
}