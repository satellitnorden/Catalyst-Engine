#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanAllocation final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	VulkanAllocation() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VulkanAllocation(const VkDeviceMemory newVulkanDeviceMemory, const uint32 newOffset) NOEXCEPT
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