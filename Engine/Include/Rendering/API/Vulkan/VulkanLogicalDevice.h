#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/Containers/StaticArray.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/API/Vulkan/VulkanQueue.h>

class VulkanLogicalDevice final
{

public:

	//Enumeration covering all queue types.
	enum class QueueType : uint8
	{
		Compute,
		Graphics,
		Present,
		Transfer,
		NumberOfQueueTypes
	};

	/*
	*	Default constructor.
	*/
	VulkanLogicalDevice() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanLogicalDevice() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan logical device.
	*/
	const VkDevice& Get() const NOEXCEPT { return vulkanLogicalDevice; }

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
	uint32 GetQueueFamilyIndex(const QueueType queueType) const NOEXCEPT { return queueFamilyIndices[INDEX(queueType)]; }

	/*
	*	Returns the queue of a queue type.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetQueue(const QueueType queueType) NOEXCEPT { return queues[INDEX(queueType)]; }

private:

	//The underlying Vulkan logical device.
	VkDevice vulkanLogicalDevice;

	//The queue family indices.
	StaticArray<uint32, INDEX(QueueType::NumberOfQueueTypes)> queueFamilyIndices;

	//The queues.
	StaticArray<VulkanQueue *RESTRICT, INDEX(QueueType::NumberOfQueueTypes)> queues;

	/*
	*	Creates the device queue create infos.
	*/
	void CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const float *const RESTRICT queuePriorities) const NOEXCEPT;

	/*
	*	Creates a physical device features.
	*/
	void CreatePhysicalDeviceFeatures(VkPhysicalDeviceFeatures &physicalDeviceFeatures) const NOEXCEPT;

	/*
	*	Creates the device create info.
	*/
	void CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *RESTRICT enabledFeatures) const NOEXCEPT;

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