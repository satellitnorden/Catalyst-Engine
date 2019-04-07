#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanAccelerationStructure final
{

public:

	/*
	*	Initializes this Vulkan acceleration structure.
	*/
	void Initialize(const VkAccelerationStructureTypeNV type, const uint32 instanceCount, const ArrayProxy<VkGeometryNV> &geometry, const VkBuffer instanceData) NOEXCEPT;

	/*
	*	Releases this Vulkan acceleration structure.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan acceleration structure.
	*/
	VkAccelerationStructureNV GetAccelerationStructure() const NOEXCEPT
	{
		return _VulkanAccelerationStructure;
	}

private:

	//The underlying Vulkan acceleration structure.
	VkAccelerationStructureNV _VulkanAccelerationStructure;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

};