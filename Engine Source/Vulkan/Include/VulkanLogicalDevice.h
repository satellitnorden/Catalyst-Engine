#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanQueue.h>

class VulkanLogicalDevice
{

public:

	/*
	*	Default constructor.
	*/
	VulkanLogicalDevice() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanLogicalDevice() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan logical device.
	*/
	const VkDevice& Get() const CATALYST_NOEXCEPT { return vulkanLogicalDevice; }

	/*
	*	Initializes this Vulkan logical device.
	*/
	void Initialize(const VulkanPhysicalDevice &vulkanPhysicalDevice) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan logical device.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the graphics queue.
	*/
	inline const VulkanQueue& GetGraphicsQueue() const CATALYST_NOEXCEPT { return graphicsQueue; }

	/*
	*	Returns the present queue.
	*/
	inline const VulkanQueue& GetPresentQueue() const CATALYST_NOEXCEPT { return presentQueue; }

	/*
	*	Returns the transfer queue.
	*/
	inline const VulkanQueue& GetTransferQueue() const CATALYST_NOEXCEPT { return transferQueue; }

private:

	//The underlying Vulkan logical device.
	VkDevice vulkanLogicalDevice;

	//The graphics queue.
	VulkanQueue graphicsQueue;

	//The present queue.
	VulkanQueue presentQueue;

	//The transfer queue.
	VulkanQueue transferQueue;

	/*
	*	Creates the device queue create infos.
	*/
	void CreateDeviceQueueCreateInfos(DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const VulkanPhysicalDevice &vulkanPhysicalDevice, const float *const CATALYST_RESTRICT queuePriorities) const CATALYST_NOEXCEPT;

	/*
	*	Creates a physical device features.
	*/
	void CreatePhysicalDeviceFeatures(VkPhysicalDeviceFeatures &physicalDeviceFeatures) const CATALYST_NOEXCEPT;

	/*
	*	Creates the device create info.
	*/
	void CreateDeviceCreateInfo(VkDeviceCreateInfo &deviceCreateInfo, const DynamicArray<VkDeviceQueueCreateInfo> &deviceQueueCreateInfos, const DynamicArray<const char *CATALYST_RESTRICT> &requiredExtensions, const VkPhysicalDeviceFeatures *CATALYST_RESTRICT enabledFeatures) const CATALYST_NOEXCEPT;

};