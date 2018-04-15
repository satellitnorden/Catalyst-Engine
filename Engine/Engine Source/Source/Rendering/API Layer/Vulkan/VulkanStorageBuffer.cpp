//Header file.
#include <Rendering/API Layer/Vulkan/VulkanStorageBuffer.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanStorageBuffer::VulkanStorageBuffer() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanStorageBuffer::~VulkanStorageBuffer() NOEXCEPT
{

}

/*
*	Initializes this Vulkan storage buffer.
*/
void VulkanStorageBuffer::Initialize(const VkDeviceSize initialStorageBufferSize) NOEXCEPT
{
	//Set the storage buffer size.
	storageBufferSize = initialStorageBufferSize;

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(initialStorageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	//Set the data in the buffer.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, VK_WHOLE_SIZE, 0, &data));

	MemoryUtilities::SetMemory(data, 0, initialStorageBufferSize);

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(storageBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanBuffer, vulkanDeviceMemory);

	//Copy the staging buffer into the storage buffer.
	VulkanUtilities::CopyBufferToBuffer(initialStorageBufferSize, stagingBuffer, vulkanBuffer);

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
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, nullptr);
}

/*
*	Returns the write descriptor set for this storage buffer.
*/
VkWriteDescriptorSet VulkanStorageBuffer::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
{
	VkWriteDescriptorSet vulkanWriteDescriptorSetCopy{ vulkanWriteDescriptorSet };

	vulkanWriteDescriptorSetCopy.dstSet = vulkanDescriptorSet.Get();
	vulkanWriteDescriptorSetCopy.dstBinding = binding;
	vulkanWriteDescriptorSetCopy.pBufferInfo = &vulkanDescriptorBufferInfo;

	return vulkanWriteDescriptorSetCopy;
}

/*
*	Creates the descriptor buffer info.
*/
void VulkanStorageBuffer::CreateDescriptorBufferInfo() NOEXCEPT
{
	vulkanDescriptorBufferInfo.buffer = vulkanBuffer;
	vulkanDescriptorBufferInfo.offset = 0;
	vulkanDescriptorBufferInfo.range = storageBufferSize;
}

/*
*	Creates the write descriptor set.
*/
void VulkanStorageBuffer::CreateWriteDescriptorSet() NOEXCEPT
{
	vulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vulkanWriteDescriptorSet.pNext = nullptr;
	vulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	vulkanWriteDescriptorSet.dstBinding = 0;
	vulkanWriteDescriptorSet.dstArrayElement = 0;
	vulkanWriteDescriptorSet.descriptorCount = 1;
	vulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	vulkanWriteDescriptorSet.pImageInfo = nullptr;
	vulkanWriteDescriptorSet.pBufferInfo = &vulkanDescriptorBufferInfo;
	vulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}