#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
#include <VulkanDescriptorSet.h>

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
	const VkBuffer& Get() const NOEXCEPT { return vulkanBuffer; }

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
	const VkDeviceSize& GetSize() const NOEXCEPT { return uniformBufferSize; }

	/*
	*	Uploads new data to the uniform buffer.
	*/
	void UploadData(void *RESTRICT newData) const NOEXCEPT;

	/*
	*	Returns the write descriptor set for this uniform buffer.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	//The size of this uniform buffer.
	VkDeviceSize uniformBufferSize;

	//The descriptor buffer info for this uniform buffer.
	VkDescriptorBufferInfo vulkanDescriptorBufferInfo;

	//The write descriptor set for this uniform buffer.
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