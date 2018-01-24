#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanAllocation final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	VulkanAllocation() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VulkanAllocation(const VkDeviceMemory newVulkanDeviceMemory, const uint32 newOffset) CATALYST_NOEXCEPT
		:
		vulkanDeviceMemory(newVulkanDeviceMemory),
		offset(newOffset)
	{

	}

	//The device memory associated with this allocation.
	VkDeviceMemory vulkanDeviceMemory;

	//The offset into the device memory associated with this allocation.
	uint32 offset;

};