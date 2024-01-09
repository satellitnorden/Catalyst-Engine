#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBufferAbstraction.h>
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanGeometryInstance.h>

class VulkanAccelerationStructure final
{

public:

	/*
	*	Initializes this Vulkan acceleration structure.
	*/
	void Initialize
	(
		const VkAccelerationStructureTypeNV type,
		const ArrayProxy<VulkanGeometryInstance> &instances,
		const ArrayProxy<VkGeometryNV> &geometry,
		VulkanCommandBuffer *const RESTRICT command_buffer
	) NOEXCEPT;

	/*
	*	Releases this Vulkan acceleration structure.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan acceleration structure.
	*/
	const VkAccelerationStructureNV &GetAccelerationStructure() const NOEXCEPT
	{
		return _VulkanAccelerationStructure;
	}

private:

	//The type.
	VkAccelerationStructureTypeNV _Type;

	//The underlying Vulkan acceleration structure.
	VkAccelerationStructureNV _VulkanAccelerationStructure;

	//The acceleration structure memory allocation.
	VmaAllocation _AccelerationStructureMemoryAllocation;

	//The acceleration structure memory allocation info.
	VmaAllocationInfo _AccelerationStructureMemoryAllocationInfo;

	//The number of instances.
	uint32 _NumberOfInstances{ 0 };

	//The instance data host buffer.
	VulkanBuffer _InstanceDataHostBuffer;

	//The instance data device buffer.
	VulkanBuffer _InstanceDataDeviceBuffer;

	//The scratch buffer.
	VulkanBuffer _ScratchBuffer;

};