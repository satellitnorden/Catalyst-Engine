//Header file.
#include <Rendering/API Layer/Vulkan/VulkanLogicalDevice.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanLogicalDevice::VulkanLogicalDevice() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanLogicalDevice::~VulkanLogicalDevice() NOEXCEPT
{

}

/*
*	Initializes this Vulkan logical device.
*/
void VulkanLogicalDevice::Initialize() NOEXCEPT
{
	//Create the device queue create info.
	static const float queuePriorities = 1.0f;

	DynamicArray<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	CreateDeviceQueueCreateInfos(deviceQueueCreateInfos, &queuePriorities);

	//Create the physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	CreatePhysicalDeviceFeatures(physicalDeviceFeatures);

	//Create the device create info.
	const DynamicArray<const char *RESTRICT> requiredExtensions{ VULKAN_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo deviceCreateInfo;
	CreateDeviceCreateInfo(deviceCreateInfo, deviceQueueCreateInfos, requiredExtensions, &physicalDeviceFeatures);

	//Create the logical device!
	VULKAN_ERROR_CHECK(vkCreateDevice(VulkanInterface::Instance->GetPhysicalDevice().Get(), &deviceCreateInfo, nullptr, &vulkanLogicalDevice));
}

/*
*	Releases this Vulkan logical device.
*/
void VulkanLogicalDevice::Release() NOEXCEPT
{
	//Destroy the Vulkan logical device.
	vkDestroyDevice(vulkanLogicalDevice, nullptr);
}

/*
*	Creates the device queue create info.
*/
void VulkanLogicalDevice::CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const float *const RESTRICT queuePriorities) const NOEXCEPT
{
	VkDeviceQueueCreateInfo newDeviceQueueCreateInfo;

	newDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	newDeviceQueueCreateInfo.pNext = nullptr;
	newDeviceQueueCreateInfo.flags = 0;
	newDeviceQueueCreateInfo.queueFamilyIndex = VulkanInterface::Instance->GetPhysicalDevice().GetGraphicsQueueFamilyIndex();
	newDeviceQueueCreateInfo.queueCount = 1;
	newDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	deviceQueueCreateInfos.EmplaceSlow(newDeviceQueueCreateInfo);
}

/*
*	Creates a physical device features.
*/
void VulkanLogicalDevice::CreatePhysicalDeviceFeatures(VkPhysicalDeviceFeatures &physicalDeviceFeatures) const NOEXCEPT
{
	physicalDeviceFeatures.robustBufferAccess = VK_FALSE;
	physicalDeviceFeatures.fullDrawIndexUint32 = VK_FALSE;
	physicalDeviceFeatures.imageCubeArray = VK_FALSE;
	physicalDeviceFeatures.independentBlend = VK_FALSE;
	physicalDeviceFeatures.geometryShader = VK_TRUE;
	physicalDeviceFeatures.tessellationShader = VK_TRUE;
	physicalDeviceFeatures.sampleRateShading = VK_FALSE;
	physicalDeviceFeatures.dualSrcBlend = VK_FALSE;
	physicalDeviceFeatures.logicOp = VK_FALSE;
	physicalDeviceFeatures.multiDrawIndirect = VK_FALSE;
	physicalDeviceFeatures.drawIndirectFirstInstance = VK_FALSE;
	physicalDeviceFeatures.depthClamp = VK_FALSE;
	physicalDeviceFeatures.depthBiasClamp = VK_FALSE;
	physicalDeviceFeatures.fillModeNonSolid = VK_FALSE;
	physicalDeviceFeatures.depthBounds = VK_FALSE;
	physicalDeviceFeatures.wideLines = VK_FALSE;
	physicalDeviceFeatures.largePoints = VK_FALSE;
	physicalDeviceFeatures.alphaToOne = VK_FALSE;
	physicalDeviceFeatures.multiViewport = VK_FALSE;
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	physicalDeviceFeatures.textureCompressionETC2 = VK_FALSE;
	physicalDeviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
	physicalDeviceFeatures.textureCompressionBC = VK_FALSE;
	physicalDeviceFeatures.occlusionQueryPrecise = VK_FALSE;
	physicalDeviceFeatures.pipelineStatisticsQuery = VK_FALSE;
	physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	physicalDeviceFeatures.fragmentStoresAndAtomics = VK_FALSE;
	physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
	physicalDeviceFeatures.shaderImageGatherExtended = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageMultisample = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;
	physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
	physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
	physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
	physicalDeviceFeatures.shaderClipDistance = VK_FALSE;
	physicalDeviceFeatures.shaderCullDistance = VK_FALSE;
	physicalDeviceFeatures.shaderFloat64 = VK_FALSE;
	physicalDeviceFeatures.shaderInt64 = VK_FALSE;
	physicalDeviceFeatures.shaderInt16 = VK_FALSE;
	physicalDeviceFeatures.shaderResourceResidency = VK_FALSE;
	physicalDeviceFeatures.shaderResourceMinLod = VK_FALSE;
	physicalDeviceFeatures.sparseBinding = VK_FALSE;
	physicalDeviceFeatures.sparseResidencyBuffer = VK_FALSE;
	physicalDeviceFeatures.sparseResidencyImage2D = VK_FALSE;
	physicalDeviceFeatures.sparseResidencyImage3D = VK_FALSE;
	physicalDeviceFeatures.sparseResidency2Samples = VK_FALSE;
	physicalDeviceFeatures.sparseResidency4Samples = VK_FALSE;
	physicalDeviceFeatures.sparseResidency8Samples = VK_FALSE;
	physicalDeviceFeatures.sparseResidency16Samples = VK_FALSE;
	physicalDeviceFeatures.sparseResidencyAliased = VK_FALSE;
	physicalDeviceFeatures.variableMultisampleRate = VK_FALSE;
	physicalDeviceFeatures.inheritedQueries = VK_FALSE;
}

/*
*	Creates the device create info.
*/
void VulkanLogicalDevice::CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *RESTRICT enabledFeatures) const NOEXCEPT
{
	//Define the required extensions.
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfos.Size());
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.Data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.Size());
	deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.Data();
	deviceCreateInfo.pEnabledFeatures = enabledFeatures;
}