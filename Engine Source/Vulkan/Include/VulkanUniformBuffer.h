#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
#include <VulkanDescriptorSet.h>

class VulkanUniformBuffer
{

public:

	/*
	*	Default constructor.
	*/
	VulkanUniformBuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanUniformBuffer() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	CATALYST_FORCE_INLINE const VkBuffer& Get() const CATALYST_NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan uniform buffer.
	*/
	void Initialize(const size_t newUniformBufferSize) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan uniform buffer.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	CATALYST_FORCE_INLINE const VkDeviceSize& GetSize() const CATALYST_NOEXCEPT { return uniformBufferSize; }

	/*
	*	Uploads new data to the uniform buffer.
	*/
	void UploadData(void *CATALYST_RESTRICT newData) const CATALYST_NOEXCEPT;

	/*
	*	Returns the write descriptor set for this uniform buffer.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT;

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
	void CreateDescriptorBufferInfo() CATALYST_NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() CATALYST_NOEXCEPT;

};