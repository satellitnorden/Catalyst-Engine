#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>

//Core.
#include <Core/Containers/Map.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>

/*
*	Initializes this Vulkan logical device.
*/
void VulkanLogicalDevice::Initialize() NOEXCEPT
{
	//Find the queue family indices.
	FindQueueFamilyIndices();

	//Create the device queue create info.
	DynamicArray<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	CreateDeviceQueueCreateInfos(deviceQueueCreateInfos);

	//Create the physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	CreatePhysicalDeviceFeatures(physicalDeviceFeatures);

	//Create the device create info.
	DynamicArray<const char *const RESTRICT> extensions;

	VulkanPlatform::RequiredLogicalDeviceExtensions(&extensions);

	for (const char *const RESTRICT extension : extensions)
	{
		PRINT_TO_OUTPUT(extension);
	}

	if (VulkanInterface::Instance->GetPhysicalDevice().HasMultiviewSupport())
	{
		extensions.Emplace(VK_KHR_MULTIVIEW_EXTENSION_NAME);
	}

	if (VulkanInterface::Instance->GetPhysicalDevice().HasRayTracingSupport())
	{
		extensions.Emplace(VK_NV_RAY_TRACING_EXTENSION_NAME);
	}

	VkDeviceCreateInfo deviceCreateInfo;
	CreateDeviceCreateInfo(deviceCreateInfo, deviceQueueCreateInfos, extensions, &physicalDeviceFeatures);

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
void VulkanLogicalDevice::CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos) const NOEXCEPT
{
	//Define constants.
	constexpr static StaticArray<float32, 3> QUEUE_PRIORITIES
	{
		1.0f,
		1.0f,
		1.0f
	};

	//Gather all unique indices and how many queues will be created from them.
	Map<uint32, uint8> queue_family_indices;

	++queue_family_indices[GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::MAIN)];
	++queue_family_indices[GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::ASYNC_COMPUTE)];
	++queue_family_indices[GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::ASYNC_TRANSFER)];

	for (const Pair<uint32, uint8> &unique_queue_family_index : queue_family_indices)
	{
		VkDeviceQueueCreateInfo newDeviceQueueCreateInfo;

		newDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		newDeviceQueueCreateInfo.pNext = nullptr;
		newDeviceQueueCreateInfo.flags = 0;
		newDeviceQueueCreateInfo.queueFamilyIndex = unique_queue_family_index._First;
		newDeviceQueueCreateInfo.queueCount = unique_queue_family_index._Second;
		newDeviceQueueCreateInfo.pQueuePriorities = QUEUE_PRIORITIES.Data();

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

	if (VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().geometryShader == VK_TRUE)
	{
		physicalDeviceFeatures.geometryShader = VK_TRUE;
	}

	else
	{
		physicalDeviceFeatures.geometryShader = VK_FALSE;
	}

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

	if (VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().samplerAnisotropy == VK_TRUE)
	{
		physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	}
	
	else
	{
		physicalDeviceFeatures.samplerAnisotropy = VK_FALSE;
	}

	physicalDeviceFeatures.textureCompressionETC2 = VK_FALSE;
	physicalDeviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
	physicalDeviceFeatures.textureCompressionBC = VK_FALSE;
	physicalDeviceFeatures.occlusionQueryPrecise = VK_FALSE;
	physicalDeviceFeatures.pipelineStatisticsQuery = VK_FALSE;
	physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;

    if (VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().fragmentStoresAndAtomics == VK_TRUE)
    {
        physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
    }

    else
    {
        physicalDeviceFeatures.fragmentStoresAndAtomics = VK_FALSE;
    }

	physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
	physicalDeviceFeatures.shaderImageGatherExtended = VK_FALSE;

    if (VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().shaderStorageImageExtendedFormats == VK_TRUE)
    {
        physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
    }

    else
    {
        physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_FALSE;
    }

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
void VulkanLogicalDevice::CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *const RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *RESTRICT enabledFeatures) const NOEXCEPT
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
	//Reset all queue indices.
	for (uint32 &queue_index : _QueueIndices)
	{
		queue_index = UINT32_MAXIMUM;
	}

	//Retrieve the queue family properties.
	uint32 queue_family_count{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queue_family_count, nullptr);

	DynamicArray<VkQueueFamilyProperties> queue_family_properties;
	queue_family_properties.Upsize<false>(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), &queue_family_count, queue_family_properties.Data());

	//Remember how many queues has been allocated from each family.
	DynamicArray<uint32> number_of_allocated_queues;
	number_of_allocated_queues.Upsize<false>(queue_family_count);
	Memory::Set(number_of_allocated_queues.Data(), 0, sizeof(uint32) * number_of_allocated_queues.Size());

#if !defined(CATALYST_CONFIGURATION_FINAL)
	{
		PRINT_TO_OUTPUT("VULKAN QUEUE INFORMATION:");

		uint32 temp_counter{ 0 };

		for (VkQueueFamilyProperties& queue_family_property : queue_family_properties)
		{
			//We want the graphics and present queue to come from the same family.
			VkBool32 has_present_support{ false };
			VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), temp_counter, VulkanInterface::Instance->GetSurface().Get(), &has_present_support));

			PRINT_TO_OUTPUT("\tQueue family #" << (temp_counter + 1) << ":");

			PRINT_TO_OUTPUT("\t\t- Number of queues: " << queue_family_property.queueCount);

			if (queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			{
				PRINT_TO_OUTPUT("\t\t- Has graphics support");
			}

			if (queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
			{
				PRINT_TO_OUTPUT("\t\t- Has compute support");
			}

			if (queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
			{
				PRINT_TO_OUTPUT("\t\t- Has transfer support");
			}

			if (has_present_support)
			{
				PRINT_TO_OUTPUT("\t\t- Has present support");
			}

			++temp_counter;
		}
	}
#endif

	/*
	*	First off, find the queue family index for the "main" queue.
	* 	According to the Vulkan specification, implementations must expose at least one queue family that has both graphics and compute functionality.
	*	Furthermore, any queue that reports graphics or compute functionality also implicitly supports transfer functionality.
	*/
	uint32 queue_family_counter{ 0 };

	for (VkQueueFamilyProperties& queue_family_property : queue_family_properties)
	{
#if !VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
		//We want the graphics and present queue to come from the same family.
		VkBool32 has_present_support{ false };
		VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), queue_family_counter, VulkanInterface::Instance->GetSurface().Get(), &has_present_support));
#endif

		if (queue_family_property.queueCount >= 1
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT
#if !VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
			&& has_present_support
#endif
			/*Transfer functionality is implicit.*/)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::MAIN)] = queue_family_counter;
			_QueueIndices[UNDERLYING(QueueType::MAIN)] = number_of_allocated_queues[queue_family_counter];
			_QueueFamilyProperties[UNDERLYING(QueueType::MAIN)] = queue_family_property;

			++number_of_allocated_queues[queue_family_counter];

			break;
		}

		++queue_family_counter;
	}

	/*
	*	Secondly, find the queue family index for the "async compute" queue.
	*/
	queue_family_counter = 0;

	for (VkQueueFamilyProperties &queue_family_property : queue_family_properties)
	{
		if (queue_family_property.queueCount >= 1
			&& number_of_allocated_queues[queue_family_counter] < queue_family_property.queueCount
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] = queue_family_counter;
			_QueueIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] = number_of_allocated_queues[queue_family_counter];
			_QueueFamilyProperties[UNDERLYING(QueueType::ASYNC_COMPUTE)] = queue_family_property;

			++number_of_allocated_queues[queue_family_counter];

			break;
		}

		++queue_family_counter;
	}

	//If a queue family index couldn't be found for the async compute queue, hijack the main queue instead.
	if (_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] == UINT32_MAXIMUM)
	{
		_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] = _QueueFamilyIndices[UNDERLYING(QueueType::MAIN)];
		_QueueIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] = _QueueIndices[UNDERLYING(QueueType::MAIN)];
		_QueueFamilyProperties[UNDERLYING(QueueType::ASYNC_COMPUTE)] = _QueueFamilyProperties[UNDERLYING(QueueType::MAIN)];
	}

	/*
	*	Thirdly, find the queue family index for the "async transfer" queue.
	*	Try to prefer queues that ONLY support transfer first.
	*/
	queue_family_counter = 0;

	for (VkQueueFamilyProperties& queue_family_property : queue_family_properties)
	{
		if (queue_family_property.queueCount >= 1
			&& number_of_allocated_queues[queue_family_counter] < queue_family_property.queueCount
			&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT
			&& !(queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			&& !(queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
		{
			_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = queue_family_counter;
			_QueueIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = number_of_allocated_queues[queue_family_counter];
			_QueueFamilyProperties[UNDERLYING(QueueType::ASYNC_TRANSFER)] = queue_family_property;

			++number_of_allocated_queues[queue_family_counter];

			break;
		}

		++queue_family_counter;
	}

	if (_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] == UINT32_MAXIMUM)
	{
		queue_family_counter = 0;

		for (VkQueueFamilyProperties& queue_family_property : queue_family_properties)
		{
			if (queue_family_property.queueCount >= 1
				&& number_of_allocated_queues[queue_family_counter] < queue_family_property.queueCount
				&& queue_family_property.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
			{
				_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = queue_family_counter;
				_QueueIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = number_of_allocated_queues[queue_family_counter];
				_QueueFamilyProperties[UNDERLYING(QueueType::ASYNC_TRANSFER)] = queue_family_property;

				++number_of_allocated_queues[queue_family_counter];

				break;
			}

			++queue_family_counter;
		}
	}

	//If a queue family index couldn't be found for the "async transfer" queue, hijack the main queue instead.
	if (_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] == UINT32_MAXIMUM)
	{
		_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = _QueueFamilyIndices[UNDERLYING(QueueType::MAIN)];
		_QueueIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] = _QueueIndices[UNDERLYING(QueueType::MAIN)];
		_QueueFamilyProperties[UNDERLYING(QueueType::ASYNC_TRANSFER)] = _QueueFamilyProperties[UNDERLYING(QueueType::MAIN)];
	}
}

/*
*	Retrieves the queues.
*/
void VulkanLogicalDevice::RetrieveQueues() NOEXCEPT
{
	//Retrieve the "main" queue.
	_Queues[UNDERLYING(QueueType::MAIN)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
	_Queues[UNDERLYING(QueueType::MAIN)]->Initialize(_QueueFamilyIndices[UNDERLYING(QueueType::MAIN)], _QueueIndices[UNDERLYING(QueueType::MAIN)]);

	//Retrieve the "async compute" queue.
	if (_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)] == _QueueFamilyIndices[UNDERLYING(QueueType::MAIN)]
		&& _QueueFamilyProperties[UNDERLYING(QueueType::MAIN)].queueCount < 2)
	{
		_Queues[UNDERLYING(QueueType::ASYNC_COMPUTE)] = _Queues[UNDERLYING(QueueType::MAIN)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::ASYNC_COMPUTE)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::ASYNC_COMPUTE)]->Initialize(_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)], _QueueIndices[UNDERLYING(QueueType::ASYNC_COMPUTE)]);
	}

	//Retrieve the "async transfer" queue.
	if (_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)] == _QueueFamilyIndices[UNDERLYING(QueueType::MAIN)]
		&& _QueueFamilyProperties[UNDERLYING(QueueType::MAIN)].queueCount < 3)
	{
		_Queues[UNDERLYING(QueueType::ASYNC_TRANSFER)] = _Queues[UNDERLYING(QueueType::MAIN)];
	}

	else
	{
		_Queues[UNDERLYING(QueueType::ASYNC_TRANSFER)] = new (Memory::Allocate(sizeof(VulkanQueue))) VulkanQueue;
		_Queues[UNDERLYING(QueueType::ASYNC_TRANSFER)]->Initialize(_QueueFamilyIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)], _QueueIndices[UNDERLYING(QueueType::ASYNC_TRANSFER)]);
	}
}

/*
*	Releases the queues.
*/
void VulkanLogicalDevice::ReleaseQueues() NOEXCEPT
{
	//Free the "main" queue.
	Memory::Free(_Queues[UNDERLYING(QueueType::MAIN)]);

	//Free the "async compute" queue.
	if (_Queues[UNDERLYING(QueueType::ASYNC_COMPUTE)] != _Queues[UNDERLYING(QueueType::MAIN)])
	{
		Memory::Free(_Queues[UNDERLYING(QueueType::ASYNC_COMPUTE)]);
	}

	//Free the "async transfer" queue.
	if (_Queues[UNDERLYING(QueueType::ASYNC_TRANSFER)] != _Queues[UNDERLYING(QueueType::MAIN)])
	{
		Memory::Free(_Queues[UNDERLYING(QueueType::ASYNC_TRANSFER)]);
	}
}
#endif