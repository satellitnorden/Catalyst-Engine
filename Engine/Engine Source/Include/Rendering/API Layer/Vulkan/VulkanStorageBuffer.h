#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>

class VulkanStorageBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanStorageBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanStorageBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return vulkanBuffer; }

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
	const VkDeviceSize& GetSize() const NOEXCEPT { return storageBufferSize; }

	/*
	*	Returns the write descriptor set for this uniform buffer.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	//The size of this storage buffer.
	VkDeviceSize storageBufferSize;

	//The descriptor buffer info for this storage buffer.
	VkDescriptorBufferInfo vulkanDescriptorBufferInfo;

	//The write descriptor set for this storage buffer.
	VkWriteDescriptorSet vulkanWriteDescriptorSet;

	/*
	*	Creates the descriptor buffer info.
	*/
	void CreateDescriptorBufferInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};