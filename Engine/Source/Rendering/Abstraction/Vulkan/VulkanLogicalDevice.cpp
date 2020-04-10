#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>

//Core.
#include <Core/Containers/Map.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan logical device.
*/
void VulkanLogicalDevice::Initialize() NOEXCEPT
{
	//Find the queue family indices.
	FindQueueFamilyIndices();

	//Create the device queue create info.
	constexpr float QUEUE_PRIORITIES{ 1.0f };

	DynamicArray<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	CreateDeviceQueueCreateInfos(deviceQueueCreateInfos, &QUEUE_PRIORITIES);

	//Create the physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	CreatePhysicalDeviceFeatures(physicalDeviceFeatures);

	//Create the device create info.
	DynamicArray<const char *RESTRICT> required_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	if (VulkanInterface::Instance->GetPhysicalDevice().HasRayTracingSupport())
	{
		required_extensions.Emplace(VK_NV_RAY_TRACING_EXTENSION_NAME);
	}

	VkDeviceCreateInfo deviceCreateInfo;
	CreateDeviceCreateInfo(deviceCreateInfo, deviceQueueCreateInfos, required_extensions, &physicalDeviceFeatures);

	//Create the logical device!
	VULKAN_ERROR_CHECK(vkCreateDevice(VulkanInterface::Instance->GetPhysicalDevice().Get(), &deviceCreateInfo, nullptr, &_VulkanLogicalDevice));

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
	vkDestroyDevice(_VulkanLogicalDevice, nullptr);
}

/*
*	Creates the device queue create info.
*/
void VulkanLogicalDevice::CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const float *const RESTRICT queuePriorities) const NOEXCEPT
{
	//Gather all unique indices and how many queues will be created from them.
	Map<uint32, uint8> uniqueQueueFamilyIndices;

	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Compute)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Present)];
	++uniqueQueueFamilyIndices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Transfer)];

	for (const Pair<uint32, uint8> &uniqueQueueFamilyIndex : uniqueQueueFamilyIndices)
	{
		VkDeviceQueueCreateInfo newDeviceQueueCreateInfo;

		newDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		newDeviceQueueCreateInfo.pNext = nullptr;
		newDeviceQueueCreateInfo.flags = 0;
		newDeviceQueueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex._First;
		newDeviceQueueCreateInfo.queueCount = 1;
		newDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

		deviceQueueCreateInfos.Emplace(newDeviceQueueCreateInfo);
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
	physicalDeviceFeatures.samplerAnisotropy = VK_FALSE;
	physicalDeviceFeatures.textureCompressionETC2 = VK_FALSE;
	physicalDeviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
	physicalDeviceFeatures.textureCompressionBC = VK_FALSE;
	physicalDeviceFeatures.occlusionQueryPrecise = VK_FALSE;
	physicalDeviceFeatures.pipelineStatisticsQuery = VK_FALSE;
	physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;
	physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
	physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
	physicalDeviceFeatures.shaderImageGatherExtended = VK_FALSE;
	physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
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
	static VkPhysicalDeviceDescriptorIndexingFeaturesEXT extension;

	extension.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	extension.pNext = nullptr;
	extension.shaderInputAttachmentArrayDynamicIndexing = VK_FALSE;
	extension.shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE;
	extension.shaderStorageTexelBufferArrayDynamicIndexing = VK_FALSE;
	extension.shaderUniformBufferArrayNonUniformIndexing = VK_FALSE;
	extension.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	extension.shaderStorageBufferArrayNonUniformIndexing = VK_FALSE;
	extension.shaderStorageImageArrayNonUniformIndexing = VK_FALSE;
	extension.shaderInputAttachmentArrayNonUniformIndexing = VK_FALSE;
	extension.shaderUniformTexelBufferArrayNonUniformIndexing = VK_FALSE;
	extension.shaderStorageTexelBufferArrayNonUniformIndexing = VK_FALSE;
	extension.descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingSampledImageUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingStorageImageUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_FALSE;
	extension.descriptorBindingPartiallyBound = VK_FALSE;
	extension.descriptorBindingVariableDescriptorCount = VK_FALSE;
	extension.runtimeDescriptorArray = VK_FALSE;

	//Define the required extensions.
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = &extension;
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
	for (uint32 &queueFamilyIndex : _QueueFamilyIndices)
	{
		queueFamilyIndex = UINT32_MAXIMUM;
	}

	//Retrieve the queue family properties.
	uint32 queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queueFamilyCount, nullptr);

	DynamicArray<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.Upsize<false>(queueFamilyCount);
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

		if (hasPresentSupport && queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] == UINT32_MAXIMUM)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] = queueFamilyCounter++;
			_QueueFamilyIndices[UNDERLYING(QueueType::Present)] = _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)];
			_QueueFamilyIndices[UNDERLYING(QueueType::Compute)] = _QueueFamilyIndices[UNDERLYING(QueueType::Present)];

			continue;
		}

		//We want a lone transfer queue.
		if (	queueFamilyProperty.queueCount > 0 &&
				queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] == UINT32_MAXIMUM &&
				!hasPresentSupport &&
				!(queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
				!(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] = queueFamilyCounter++;

			continue;
		}

		++queueFamilyCounter;
	}

	for (int64 i = queueFamilyProperties.LastIndex(); i >= 0; --i)
	{
		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Compute)] == UINT32_MAXIMUM)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Compute)] = static_cast<uint32>(i);

			continue;
		}

		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] == UINT32_MAXIMUM)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] = static_cast<uint32>(i);

			continue;
		}

		VkBool32 hasPresentSupport{ false };
		VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), static_cast<uint32>(i), VulkanInterface::Instance->GetSurface().Get(), &hasPresentSupport));

		if (queueFamilyProperties[i].queueCount > 0 && hasPresentSupport && _QueueFamilyIndices[UNDERLYING(QueueType::Present)] == UINT32_MAXIMUM)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Present)] = static_cast<uint32>(i);

			continue;
		}

		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] == UINT32_MAXIMUM)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] = static_cast<uint32>(i);

			continue;
		}
	}

	if (	_QueueFamilyIndices[UNDERLYING(QueueType::Compute)] == UINT32_MAXIMUM ||
			_QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] == UINT32_MAXIMUM ||
			_QueueFamilyIndices[UNDERLYING(QueueType::Present)] == UINT32_MAXIMUM ||
			_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] == UINT32_MAXIMUM)
	{
		queueFamilyCounter = 0;

		for (auto &queueFamilyProperty : queueFamilyProperties)
		{
			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Compute)] == UINT32_MAXIMUM)
			{
				_QueueFamilyIndices[UNDERLYING(QueueType::Compute)] = queueFamilyCounter;
			}

			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] == UINT32_MAXIMUM)
			{
				_QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] = queueFamilyCounter;
			}

			VkBool32 presentSupport = false;

			VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), queueFamilyCounter, VulkanInterface::Instance->GetSurface().Get(), &presentSupport));

			if (queueFamilyProperty.queueCount > 0 && presentSupport && _QueueFamilyIndices[UNDERLYING(QueueType::Present)] == UINT32_MAXIMUM)
			{
				_QueueFamilyIndices[UNDERLYING(QueueType::Present)] = queueFamilyCounter;
			}

			if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT && _QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] == UINT32_MAXIMUM)
			{
				_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] = queueFamilyCounter;
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
	const uint32 computeQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Compute)] };
	const uint32 graphicsQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] };
	const uint32 presentQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Present)] };
	const uint32 transferQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] };

	//Retrieve the compute queue.
	_Queues[UNDERLYING(QueueType::Compute)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
	_Queues[UNDERLYING(QueueType::Compute)]->Initialize(computeQueueFamilyIndex);

	//Retrieve the graphics queue.
	if (graphicsQueueFamilyIndex == computeQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Graphics)] = _Queues[UNDERLYING(QueueType::Compute)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::Graphics)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::Graphics)]->Initialize(graphicsQueueFamilyIndex);
	}

	//Retrieve the present queue.
	if (presentQueueFamilyIndex == computeQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Present)] = _Queues[UNDERLYING(QueueType::Compute)];
	}

	else if (presentQueueFamilyIndex == graphicsQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Present)] = _Queues[UNDERLYING(QueueType::Graphics)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::Present)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::Present)]->Initialize(presentQueueFamilyIndex);
	}

	//Retrieve the transfer queue.
	if (transferQueueFamilyIndex == computeQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = _Queues[UNDERLYING(QueueType::Compute)];
	}

	else if (transferQueueFamilyIndex == graphicsQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = _Queues[UNDERLYING(QueueType::Graphics)];
	}

	else if (transferQueueFamilyIndex == presentQueueFamilyIndex)
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = _Queues[UNDERLYING(QueueType::Present)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::Transfer)]->Initialize(transferQueueFamilyIndex);
	}
}

/*
*	Releases the queues.
*/
void VulkanLogicalDevice::ReleaseQueues() NOEXCEPT
{
	//Store the queue family indices.
	const uint32 computeQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Compute)] };
	const uint32 graphicsQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] };
	const uint32 presentQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Present)] };
	const uint32 transferQueueFamilyIndex{ _QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] };

	//Destroy the compute queue.
	Memory::Free(_Queues[UNDERLYING(QueueType::Compute)]);

	//Destroy the graphics queue.
	if (graphicsQueueFamilyIndex != computeQueueFamilyIndex)
	{
		Memory::Free(_Queues[UNDERLYING(QueueType::Graphics)]);
	}

	//Destroy the present queue.
	if (	presentQueueFamilyIndex != computeQueueFamilyIndex &&
			presentQueueFamilyIndex != graphicsQueueFamilyIndex)
	{
		Memory::Free(_Queues[UNDERLYING(QueueType::Present)]);
	}

	//Destroy the transfer queue.
	if (	transferQueueFamilyIndex != computeQueueFamilyIndex &&
			transferQueueFamilyIndex != graphicsQueueFamilyIndex &&
			transferQueueFamilyIndex != presentQueueFamilyIndex)
	{
		Memory::Free(_Queues[UNDERLYING(QueueType::Transfer)]);
	}
}
#endif