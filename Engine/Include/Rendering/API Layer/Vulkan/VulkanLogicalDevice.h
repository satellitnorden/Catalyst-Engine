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

private:

	//The underlying Vulkan logical device.
	VkDevice vulkanLogicalDevice;

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

};