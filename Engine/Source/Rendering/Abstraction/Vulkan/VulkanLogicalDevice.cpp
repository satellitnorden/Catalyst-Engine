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
	Map<uint32, uint8> queue_family_indices;

	++queue_family_indices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Compute)];
	++queue_family_indices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics)];
	++queue_family_indices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Present)];
	++queue_family_indices[VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Transfer)];

	for (const Pair<uint32, uint8> &unique_queue_family_index : queue_family_indices)
	{
		VkDeviceQueueCreateInfo newDeviceQueueCreateInfo;

		newDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		newDeviceQueueCreateInfo.pNext = nullptr;
		newDeviceQueueCreateInfo.flags = 0;
		newDeviceQueueCreateInfo.queueFamilyIndex = unique_queue_family_index._First;
		newDeviceQueueCreateInfo.queueCount = unique_queue_family_index._Second;
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
	for (uint32 &queue_family_index : _QueueFamilyIndices)
	{
		queue_family_index = UINT32_MAXIMUM;
	}

	//Retrieve the queue family properties.
	uint32 queue_family_count{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queue_family_count, nullptr);

	DynamicArray<VkQueueFamilyProperties> queue_family_properties;
	queue_family_properties.Upsize<false>(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queue_family_count, queue_family_properties.Data());

	/*
	*	Does a couple of runs here.
	*	Ideally a queue family exists with all kinds of queue types available.
	*/
	uint32 queue_family_counter{ 0 };

	//First run, try to find optimal indices; All queues coming from the same family.
	for (VkQueueFamilyProperties &queue_family_property : queue_family_properties)
	{
		//We want the graphics and present queue to come from the same family.
		VkBool32 has_present_support{ false };
		VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), queue_family_counter, VulkanInterface::Instance->GetSurface().Get(), &has_present_support));

		if (queue_family_property.queueCount >= 2
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT
			&& has_present_support
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::Compute)] = queue_family_counter;
			_QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] = queue_family_counter;
			_QueueFamilyIndices[UNDERLYING(QueueType::Present)] = queue_family_counter;
			_QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] = queue_family_counter;

			break;
		}

		++queue_family_counter;
	}

	//Check if any queue family index has not been set yet.
	for (const uint32 queue_family_index : _QueueFamilyIndices)
	{
		ASSERT(queue_family_index != UINT32_MAXIMUM, "Queue family index not set!");
	}
}

/*
*	Retrieves the queues.
*/
void VulkanLogicalDevice::RetrieveQueues() NOEXCEPT
{
	//Store the queue family indices.
	const uint32 compute_queue_family_index{ _QueueFamilyIndices[UNDERLYING(QueueType::Compute)] };
	const uint32 graphics_queue_family_index{ _QueueFamilyIndices[UNDERLYING(QueueType::Graphics)] };
	const uint32 present_queue_family_index{ _QueueFamilyIndices[UNDERLYING(QueueType::Present)] };
	const uint32 transfer_queue_family_index{ _QueueFamilyIndices[UNDERLYING(QueueType::Transfer)] };

	//Retrieve the compute queue.
	_Queues[UNDERLYING(QueueType::Compute)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
	_Queues[UNDERLYING(QueueType::Compute)]->Initialize(compute_queue_family_index);

	//Retrieve the graphics queue.
	_Queues[UNDERLYING(QueueType::Graphics)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
	_Queues[UNDERLYING(QueueType::Graphics)]->Initialize(graphics_queue_family_index);

	//Retrieve the present queue. Prefer if the present queue and the graphis queue are the same.
	if (present_queue_family_index == graphics_queue_family_index)
	{
		_Queues[UNDERLYING(QueueType::Present)] = _Queues[UNDERLYING(QueueType::Graphics)];
	}
	
	else
	{
		_Queues[UNDERLYING(QueueType::Present)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::Present)]->Initialize(present_queue_family_index);
	}

	//Retrieve the transfer queue. Prefer if the transer queue and the graphis queue are the same.
	if (transfer_queue_family_index == graphics_queue_family_index)
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = _Queues[UNDERLYING(QueueType::Graphics)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::Transfer)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::Transfer)]->Initialize(transfer_queue_family_index);
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