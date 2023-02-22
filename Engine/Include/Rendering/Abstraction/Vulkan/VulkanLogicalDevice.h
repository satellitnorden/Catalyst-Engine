#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanQueue.h>

class VulkanLogicalDevice final
{

public:

	//Enumeration covering all queue types.
	enum class QueueType : uint8
	{
		/*
		*	This is the "main" queue, where all primary rendering happens.
		*	This queue will also have present functionality.
		*/
		MAIN,

		/*
		*	This is the "async compute" queue, where compute shaders can happen in the background.
		*/
		ASYNC_COMPUTE,

		/*
		*	This is the "async transfer" queue, where transferring resources to/from the GPU can happen in the background.
		*/
		ASYNC_TRANSFER,

		NUMBER_OF_QUEUE_TYPES
	};

	/*
	*	Returns the underlying Vulkan logical device.
	*/
	const VkDevice& Get() const NOEXCEPT { return _VulkanLogicalDevice; }

	/*
	*	Initializes this Vulkan logical device.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan logical device.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the queue family index of a queue type.
	*/
	uint32 GetQueueFamilyIndex(const QueueType queueType) const NOEXCEPT { return _QueueFamilyIndices[UNDERLYING(queueType)]; }

	/*
	*	Returns the queue family properties of a queue type.
	*/
	VkQueueFamilyProperties GetQueueFamilyProperties(const QueueType queueType) const NOEXCEPT { return _QueueFamilyProperties[UNDERLYING(queueType)]; }

	/*
	*	Returns the queue of a queue type.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetQueue(const QueueType queueType) NOEXCEPT { return _Queues[UNDERLYING(queueType)]; }

private:

	//The underlying Vulkan logical device.
	VkDevice _VulkanLogicalDevice;

	//The queue family indices.
	StaticArray<uint32, UNDERLYING(QueueType::NUMBER_OF_QUEUE_TYPES)> _QueueFamilyIndices;

	//The queue indices.
	StaticArray<uint32, UNDERLYING(QueueType::NUMBER_OF_QUEUE_TYPES)> _QueueIndices;

	//The queue family properties.
	StaticArray<VkQueueFamilyProperties, UNDERLYING(QueueType::NUMBER_OF_QUEUE_TYPES)> _QueueFamilyProperties;

	//The queues.
	StaticArray<VulkanQueue *RESTRICT, UNDERLYING(QueueType::NUMBER_OF_QUEUE_TYPES)> _Queues;

	/*
	*	Creates the device queue create infos.
	*/
	void CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos) const NOEXCEPT;

	/*
	*	Creates a physical device features.
	*/
	void CreatePhysicalDeviceFeatures(VkPhysicalDeviceFeatures &physicalDeviceFeatures) const NOEXCEPT;

	/*
	*	Creates the device create info.
	*/
	void CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *const RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *RESTRICT enabledFeatures) const NOEXCEPT;

	/*
	*	Finds the queue family indices.
	*/
	void FindQueueFamilyIndices() NOEXCEPT;

	/*
	*	Retrieves the queues.
	*/
	void RetrieveQueues() NOEXCEPT;

	/*
	*	Releases the queues.
	*/
	void ReleaseQueues() NOEXCEPT;

};
#endif