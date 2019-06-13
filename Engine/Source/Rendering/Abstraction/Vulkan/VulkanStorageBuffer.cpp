#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanStorageBuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan storage buffer.
*/
void VulkanStorageBuffer::Initialize(const VkDeviceSize initialStorageBufferSize) NOEXCEPT
{
	//Set the storage buffer size.
	_StorageBufferSize = initialStorageBufferSize;

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(initialStorageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	//Set the data in the buffer.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, VK_WHOLE_SIZE, 0, &data));

	Memory::SetMemory(data, 0, initialStorageBufferSize);

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(_StorageBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanBuffer, _VulkanDeviceMemory);

	//Copy the staging buffer into the storage buffer.
	VulkanUtilities::CopyBufferToBuffer(initialStorageBufferSize, stagingBuffer, _VulkanBuffer, nullptr);

	//Release the staging buffer.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

	//Create the descriptor buffer info.
	CreateDescriptorBufferInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan storage buffer.
*/
void VulkanStorageBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanBuffer, nullptr);
}

/*
*	Returns the write descriptor set for this storage buffer.
*/
VkWriteDescriptorSet VulkanStorageBuffer::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
{
	VkWriteDescriptorSet vulkanWriteDescriptorSetCopy{ _VulkanWriteDescriptorSet };

	vulkanWriteDescriptorSetCopy.dstSet = vulkanDescriptorSet.Get();
	vulkanWriteDescriptorSetCopy.dstBinding = binding;
	vulkanWriteDescriptorSetCopy.pBufferInfo = &_VulkanDescriptorBufferInfo;

	return vulkanWriteDescriptorSetCopy;
}

/*
*	Creates the descriptor buffer info.
*/
void VulkanStorageBuffer::CreateDescriptorBufferInfo() NOEXCEPT
{
	_VulkanDescriptorBufferInfo.buffer = _VulkanBuffer;
	_VulkanDescriptorBufferInfo.offset = 0;
	_VulkanDescriptorBufferInfo.range = _StorageBufferSize;
}

/*
*	Creates the write descriptor set.
*/
void VulkanStorageBuffer::CreateWriteDescriptorSet() NOEXCEPT
{
	_VulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	_VulkanWriteDescriptorSet.pNext = nullptr;
	_VulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	_VulkanWriteDescriptorSet.dstBinding = 0;
	_VulkanWriteDescriptorSet.dstArrayElement = 0;
	_VulkanWriteDescriptorSet.descriptorCount = 1;
	_VulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	_VulkanWriteDescriptorSet.pImageInfo = nullptr;
	_VulkanWriteDescriptorSet.pBufferInfo = &_VulkanDescriptorBufferInfo;
	_VulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}
#endif