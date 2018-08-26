//Header file.
#include <Rendering/API/Vulkan/VulkanLogicalDevice.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

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
	//Find the queue family indices.
	FindQueueFamilyIndices();

	//Create the device queue create info.
	static const float queuePriorities = 1.0f;

	DynamicArray<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	CreateDeviceQueueCreateInfos(deviceQueueCreateInfos, &queuePriorities);

	//Create the physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	CreatePhysicalDeviceFeatures(physicalDeviceFeatures);

	//Create the device create info.
	const DynamicArray<const char *RESTRICT> requiredExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo deviceCreateInfo;
	CreateDeviceCreateInfo(deviceCreateInfo, deviceQueueCreateInfos, requiredExtensions, &physicalDeviceFeatures);

	//Create the logical device!
	VULKAN_ERROR_CHECK(vkCreateDevice(VulkanInterface::Instance->GetPhysicalDevice().Get(), &deviceCreateInfo, nullptr, &vulkanLogicalDevice));

	//Retrieve the queues.
	RetrieveQueues();
}

/*
*	Releases this Vulkan logical device.
*/
void VulkanLogicalDevice::Release() NOEXCEPT
{
	//Release the queues.
	ReleaseQueues();

	//Destroy the Vulkan logical device.
	vkDestroyDevice(vulkanLogicalDevice, nullptr);
}

/*
*	Creates the device queue create info.
*/
void VulkanLogicalDevice::CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const float *const RESTRICT queuePriorities) const NOEXCEPT
{
	//Gather all unique indices and how many queues will be created from them.
	std::map<uint32, uint8> uniqueQueueFamilyIndices;

	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Compute)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Present)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Transfer)];

	for (const auto &uniqueQueueFamilyIndex : uniqueQueueFamilyIndices)
	{
		VkDeviceQueueCreateInfo newDeviceQueueCreateInfo;

		newDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		newDeviceQueueCreateInfo.pNext = nullptr;
		newDeviceQueueCreateInfo.flags = 0;
		newDeviceQueueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex.first;
		newDeviceQueueCreateInfo.queueCount = 1;
		newDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

		deviceQueueCreateInfos.EmplaceSlow(newDeviceQueueCreateInfo);
	}
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
	physicalDeviceFeatures.samplerAnisotropy = VK_FALSE;
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
	deviceCreateInfo.ppEnabledExtensionNames = (const char *const *) requiredExtensions.Data();
	deviceCreateInfo.pEnabledFeatures = enabledFeatures;
}

/*
*	Finds the queue family indices.
*/
void VulkanLogicalDevice::FindQueueFamilyIndices() NOEXCEPT
{
	//Reset all queue family indices.
	for (uint32 &queueFamilyIndex : queueFamilyIndices)
	{
		queueFamilyIndex = UINT32_MAXIMUM;
	}

	//Retrieve the queue family properties.
	uint32 queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queueFamilyCount, nullptr);

	DynamicArray<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.UpsizeFast(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queueFamilyCount, queueFamilyProperties.Data());

	/*
	*	Does three runs here.
	*	First tries to find optimal queue family indices based on certain criteria.
	*	Second, tries to find a unique queue family index for each type of queue.
	*	If this can't be done, do a third run to fill in the blanks.
	*/
	uint32 queueFamilyCounter{ 0 };

	//First run, try to find optimal indices; Graphics and present from the same queue family, lone transfer queue family.
	for (VkQueueFamilyProperties &queueFamilyProperty : queueFamilyProperties)
	{
		//We want the graphics and present queue to come from the same family.
		VkBool32 hasPresentSupport{ false };
		VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), queueFamilyCounter, VulkanInterface::Instance->GetSurface().Get(), &hasPresentSupport));

		if (hasPresentSupport && queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamilyIndices[INDEX(QueueType::Graphics)] == UINT32_MAXIMUM)
		{
			queueFamilyIndices[INDEX(QueueType::Graphics)] = queueFamilyCounter++;
			queueFamilyIndices[INDEX(QueueType::Present)] = queueFamilyIndices[INDEX(QueueType::Graphics)];

			continue;
		}

		//We want a lone transfer queue.
		if (	queueFamilyProperty.queueCount > 0 &&
				queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				queueFamilyIndices[INDEX(QueueType::Transfer)] == UINT32_MAXIMUM &&
				!hasPresentSupport &&
				!(queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
				!(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			queueFamilyIndices[INDEX(QueueType::Transfer)] = queueFamilyCounter++;

			continue;
		}

		++queueFamilyCounter;
	}

	for (int64 i = queueFamilyProperties.LastIndex(); i >= 0; --i)
	{
		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && queueFamilyIndices[INDEX(QueueType::Compute)] == UINT32_MAXIMUM)
		{
			queueFamilyIndices[INDEX(QueueType::Compute)] = static_cast<uint32>(i);

			continue;
		}

		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamilyIndices[INDEX(QueueType::Graphics)] == UINT32_MAXIMUM)
		{
			queueFamilyIndices[INDEX(QueueType::Graphics)] = static_cast<uint32>(i);

			continue;
		}

		VkBool32 hasPresentSupport{ false };
		VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), static_cast<uint32>(i), VulkanInterface::Instance->GetSurface().Get(), &hasPresentSupport));

		if (queueFamilyProperties[i].queueCount > 0 && hasPresentSupport && queueFamilyIndices[INDEX(QueueType::Present)] == UINT32_MAXIMUM)
		{
			queueFamilyIndices[INDEX(QueueType::Present)] = static_cast<uint32>(i);

			continue;
		}

		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueFamilyIndices[INDEX(QueueType::Transfer)] == UINT32_MAXIMUM)
		{
			queueFamilyIndices[INDEX(QueueType::Transfer)] = static_cast<uint32>(i);

			continue;
		}
	}

	if (	queueFamilyIndices[INDEX(QueueType::Compute)] == UINT32_MAXIMUM ||
			queueFamilyIndices[INDEX(QueueType::Graphics)] == UINT32_MAXIMUM ||
			queueFamilyIndices[INDEX(QueueType::Present)] == UINT32_MAXIMUM ||
			queueFamilyIndices[INDEX(QueueType::Transfer)] == UINT32_MAXIMUM)
	{
		queueFamilyCounter = 0;

		for (auto &queueFamilyProperty : queueFamilyProperties)
		{
			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT && queueFamilyIndices[INDEX(QueueType::Compute)] == UINT32_MAXIMUM)
			{
				queueFamilyIndices[INDEX(QueueType::Compute)] = queueFamilyCounter;
			}

			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamilyIndices[INDEX(QueueType::Graphics)] == UINT32_MAXIMUM)
			{
				queueFamilyIndices[INDEX(QueueType::Graphics)] = queueFamilyCounter;
			}

			VkBool32 presentSupport = false;

			VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), queueFamilyCounter, VulkanInterface::Instance->GetSurface().Get(), &presentSupport));

			if (queueFamilyProperty.queueCount > 0 && presentSupport && queueFamilyIndices[INDEX(QueueType::Present)] == UINT32_MAXIMUM)
			{
				queueFamilyIndices[INDEX(QueueType::Present)] = queueFamilyCounter;
			}

			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT && queueFamilyIndices[INDEX(QueueType::Transfer)] == UINT32_MAXIMUM)
			{
				queueFamilyIndices[INDEX(QueueType::Transfer)] = queueFamilyCounter;
			}

			++queueFamilyCounter;
		}
	}
}

/*
*	Retrieves the queues.
*/
void VulkanLogicalDevice::RetrieveQueues() NOEXCEPT
{
	//Store the queue family indices.
	const uint32 computeQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Compute)] };
	const uint32 graphicsQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Graphics)] };
	const uint32 presentQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Present)] };
	const uint32 transferQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Transfer)] };

	//Retrieve the compute queue.
	queues[INDEX(QueueType::Compute)] = new VulkanQueue();
	queues[INDEX(QueueType::Compute)]->Initialize(computeQueueFamilyIndex);

	//Retrieve the graphics queue.
	if (graphicsQueueFamilyIndex == computeQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Graphics)] = queues[INDEX(QueueType::Compute)];
	}

	else
	{
		queues[INDEX(QueueType::Graphics)] = new VulkanQueue();
		queues[INDEX(QueueType::Graphics)]->Initialize(graphicsQueueFamilyIndex);
	}

	//Retrieve the present queue.
	if (presentQueueFamilyIndex == computeQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Present)] = queues[INDEX(QueueType::Compute)];
	}

	else if (presentQueueFamilyIndex == graphicsQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Present)] = queues[INDEX(QueueType::Graphics)];
	}

	else
	{
		queues[INDEX(QueueType::Present)] = new VulkanQueue();
		queues[INDEX(QueueType::Present)]->Initialize(presentQueueFamilyIndex);
	}

	//Retrieve the transfer queue.
	if (transferQueueFamilyIndex == computeQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Transfer)] = queues[INDEX(QueueType::Compute)];
	}

	else if (transferQueueFamilyIndex == graphicsQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Transfer)] = queues[INDEX(QueueType::Graphics)];
	}

	else if (transferQueueFamilyIndex == presentQueueFamilyIndex)
	{
		queues[INDEX(QueueType::Transfer)] = queues[INDEX(QueueType::Present)];
	}

	else
	{
		queues[INDEX(QueueType::Transfer)] = new VulkanQueue();
		queues[INDEX(QueueType::Transfer)]->Initialize(transferQueueFamilyIndex);
	}
}

/*
*	Releases the queues.
*/
void VulkanLogicalDevice::ReleaseQueues() NOEXCEPT
{
	//Store the queue family indices.
	const uint32 computeQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Compute)] };
	const uint32 graphicsQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Graphics)] };
	const uint32 presentQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Present)] };
	const uint32 transferQueueFamilyIndex{ queueFamilyIndices[INDEX(QueueType::Transfer)] };

	//Destroy the compute queue.
	delete queues[INDEX(QueueType::Compute)];

	//Destroy the graphics queue.
	if (graphicsQueueFamilyIndex != computeQueueFamilyIndex)
	{
		delete queues[INDEX(QueueType::Graphics)];
	}

	//Destroy the present queue.
	if (	presentQueueFamilyIndex != computeQueueFamilyIndex &&
			presentQueueFamilyIndex != graphicsQueueFamilyIndex)
	{
		delete queues[INDEX(QueueType::Present)];
	}

	//Destroy the transfer queue.
	if (	transferQueueFamilyIndex != computeQueueFamilyIndex &&
			transferQueueFamilyIndex != graphicsQueueFamilyIndex &&
			transferQueueFamilyIndex != presentQueueFamilyIndex)
	{
		delete queues[INDEX(QueueType::Transfer)];
	}
}