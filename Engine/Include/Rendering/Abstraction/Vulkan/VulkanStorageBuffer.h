#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>

class VulkanStorageBuffer final
{

public:

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return _VulkanBuffer; }

	/*
	*	Initializes this Vulkan storage buffer.
	*/
	void Initialize(const VkDeviceSize initialStorageBufferSize) NOEXCEPT;

	/*
	*	Releases this Vulkan storage buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the size of this storage buffer.
	*/
	const VkDeviceSize& GetSize() const NOEXCEPT { return _StorageBufferSize; }

	/*
	*	Returns the write descriptor set for this uniform buffer.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer _VulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The size of this storage buffer.
	VkDeviceSize _StorageBufferSize;

	//The descriptor buffer info for this storage buffer.
	VkDescriptorBufferInfo _VulkanDescriptorBufferInfo;

	//The write descriptor set for this storage buffer.
	VkWriteDescriptorSet _VulkanWriteDescriptorSet;

	/*
	*	Creates the descriptor buffer info.
	*/
	void CreateDescriptorBufferInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};
#endif