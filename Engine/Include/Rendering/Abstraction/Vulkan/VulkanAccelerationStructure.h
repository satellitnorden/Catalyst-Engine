#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanGeometryInstance.h>

class VulkanAccelerationStructure final
{

public:

	/*
	*	Initializes this Vulkan acceleration structure.
	*/
	void Initialize(const VkAccelerationStructureTypeNV type, const ArrayProxy<VulkanGeometryInstance> &instances, const ArrayProxy<VkGeometryNV> &geometry) NOEXCEPT;

	/*
	*	Releases this Vulkan acceleration structure.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan acceleration structure.
	*/
	const VkAccelerationStructureNV& GetAccelerationStructure() const NOEXCEPT
	{
		return _VulkanAccelerationStructure;
	}

private:

	//The underlying Vulkan acceleration structure.
	VkAccelerationStructureNV _VulkanAccelerationStructure;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The instance data buffer.
	VulkanBuffer _InstanceDataBuffer;

	//Denotes if this acceleration structure has instance data or not.
	bool _HasInstanceData{ false };

};