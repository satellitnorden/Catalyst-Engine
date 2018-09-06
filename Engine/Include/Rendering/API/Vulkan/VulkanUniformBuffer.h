#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSet.h>

class VulkanUniformBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanUniformBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanUniformBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return _VulkanBuffer; }

	/*
	*	Initializes this Vulkan uniform buffer.
	*/
	void Initialize(const VkDeviceSize newUniformBufferSize) NOEXCEPT;

	/*
	*	Releases this Vulkan uniform buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkDeviceSize& GetSize() const NOEXCEPT { return _VniformBufferSize; }

	/*
	*	Uploads new data to the uniform buffer.
	*/
	void UploadData(const void *RESTRICT newData) const NOEXCEPT;

	/*
	*	Returns the write descriptor set for this uniform buffer.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer _VulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The size of this uniform buffer.
	VkDeviceSize _VniformBufferSize;

	//The descriptor buffer info for this uniform buffer.
	VkDescriptorBufferInfo _VulkanDescriptorBufferInfo;

	//The write descriptor set for this uniform buffer.
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