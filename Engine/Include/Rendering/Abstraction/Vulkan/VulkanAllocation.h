#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

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
		_VulkanDeviceMemory(newVulkanDeviceMemory),
		_Offset(newOffset)
	{

	}

	//The device memory associated with this allocation.
	VkDeviceMemory _VulkanDeviceMemory;

	//The offset into the device memory associated with this allocation.
	uint32 _Offset;

};
#endif