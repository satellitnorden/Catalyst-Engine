//Header file.
#include <VulkanLogicalDevice.h>

/*
*	Default constructor.
*/
VulkanLogicalDevice::VulkanLogicalDevice() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanLogicalDevice::~VulkanLogicalDevice() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan logical device.
*/
void VulkanLogicalDevice::Initialize(const VulkanPhysicalDevice &vulkanPhysicalDevice) CATALYST_NOEXCEPT
{
	//Create the device queue create info.
	const float queuePriorities = 1.0f;

	DynamicArray<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	CreateDeviceQueueCreateInfos(deviceQueueCreateInfos, vulkanPhysicalDevice, &queuePriorities);

	//Create the physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	CreatePhysicalDeviceFeatures(physicalDeviceFeatures);

	//Create the device create info.
	const DynamicArray<const char *CATALYST_RESTRICT> requiredExtensions{ VULKAN_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo deviceCreateInfo;
	CreateDeviceCreateInfo(deviceCreateInfo, deviceQueueCreateInfos, requiredExtensions, &physicalDeviceFeatures);

	//Create the logical device!
	VkResult result = vkCreateDevice(vulkanPhysicalDevice.Get(), &deviceCreateInfo, nullptr, &vulkanLogicalDevice);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif

	//Initialize the graphics queue.
	VkQueue newGraphicsQueue;
	vkGetDeviceQueue(vulkanLogicalDevice, vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex(), 0, &newGraphicsQueue);
	graphicsQueue.Initialize(newGraphicsQueue);

	//Initialize the present queue.
	VkQueue newPresentQueue;
	vkGetDeviceQueue(vulkanLogicalDevice, vulkanPhysicalDevice.GetPresentQueueFamilyIndex(), 0, &newPresentQueue);
	presentQueue.Initialize(newPresentQueue);

	//Initialize the transfer queue.
	VkQueue newTransferQueue;
	vkGetDeviceQueue(vulkanLogicalDevice, vulkanPhysicalDevice.GetTransferQueueFamilyIndex(), 0, &newTransferQueue);
	transferQueue.Initialize(newTransferQueue);
}

/*
*	Releases this Vulkan logical device.
*/
void VulkanLogicalDevice::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan logical device.
	vkDestroyDevice(vulkanLogicalDevice, nullptr);
}

/*
*	Creates the device queue create info.
*/
void VulkanLogicalDevice::CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const VulkanPhysicalDevice &vulkanPhysicalDevice, const float *const CATALYST_RESTRICT queuePriorities) const CATALYST_NOEXCEPT
{
	VkDeviceQueueCreateInfo graphicsDeviceQueueCreateInfo;

	graphicsDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsDeviceQueueCreateInfo.pNext = nullptr;
	graphicsDeviceQueueCreateInfo.flags = 0;
	graphicsDeviceQueueCreateInfo.queueFamilyIndex = vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex();
	graphicsDeviceQueueCreateInfo.queueCount = 1;
	graphicsDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceQueueCreateInfo presentDeviceQueueCreateInfo;

	presentDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	presentDeviceQueueCreateInfo.pNext = nullptr;
	presentDeviceQueueCreateInfo.flags = 0;
	presentDeviceQueueCreateInfo.queueFamilyIndex = vulkanPhysicalDevice.GetPresentQueueFamilyIndex();
	presentDeviceQueueCreateInfo.queueCount = 1;
	presentDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	deviceQueueCreateInfos.Reserve(2);

	deviceQueueCreateInfos.EmplaceUnsafe(graphicsDeviceQueueCreateInfo);
	deviceQueueCreateInfos.EmplaceUnsafe(presentDeviceQueueCreateInfo);
}

/*
*	Creates a physical device features.
*/
void VulkanLogicalDevice::CreatePhysicalDeviceFeatures(VkPhysicalDeviceFeatures &physicalDeviceFeatures) const CATALYST_NOEXCEPT
{
	physicalDeviceFeatures.robustBufferAccess = VK_FALSE;
	physicalDeviceFeatures.fullDrawIndexUint32 = VK_FALSE;
	physicalDeviceFeatures.imageCubeArray = VK_FALSE;
	physicalDeviceFeatures.independentBlend = VK_FALSE;
	physicalDeviceFeatures.geometryShader = VK_FALSE;
	physicalDeviceFeatures.tessellationShader = VK_FALSE;
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
	physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;
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
void VulkanLogicalDevice::CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *CATALYST_RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *CATALYST_RESTRICT enabledFeatures) const CATALYST_NOEXCEPT
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