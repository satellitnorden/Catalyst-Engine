#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>
#include <Rendering/API Layer/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/API Layer/Vulkan/VulkanQueue.h>

class VulkanLogicalDevice final
{

public:

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
	*	Returns the compute queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetComputeQueue() NOEXCEPT { return computeQueue; }

	/*
	*	Returns the graphics queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetGraphicsQueue() NOEXCEPT { return graphicsQueue; }

	/*
	*	Returns the present queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetPresentQueue() NOEXCEPT { return presentQueue; }

	/*
	*	Returns the transfer queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetTransferQueue() NOEXCEPT { return transferQueue; }

private:

	//The underlying Vulkan logical device.
	VkDevice vulkanLogicalDevice;

	//The compute queue.
	VulkanQueue *RESTRICT computeQueue;

	//The graphics queue.
	VulkanQueue *RESTRICT graphicsQueue;

	//The present queue.
	VulkanQueue *RESTRICT presentQueue;

	//The transfer queue.
	VulkanQueue *RESTRICT transferQueue;

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
	*	Retrieves the queues.
	*/
	void RetrieveQueues() NOEXCEPT;

	/*
	*	Releases the queues.
	*/
	void ReleaseQueues() NOEXCEPT;

};