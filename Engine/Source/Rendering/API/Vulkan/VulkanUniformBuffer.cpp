//Header file.
#include <Rendering/API/Vulkan/VulkanUniformBuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanUniformBuffer::VulkanUniformBuffer() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanUniformBuffer::~VulkanUniformBuffer() NOEXCEPT
{

}

/*
*	Initializes this Vulkan uniform buffer.
*/
void VulkanUniformBuffer::Initialize(const VkDeviceSize newUniformBufferSize) NOEXCEPT
{
	//Set the uniform buffer properties.
	_VniformBufferSize = newUniformBufferSize;

	//Create the Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(_VniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _VulkanBuffer, _VulkanDeviceMemory);

	//Create the descriptor buffer info.
	CreateDescriptorBufferInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan uniform buffer.
*/
void VulkanUniformBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanBuffer, nullptr);
}

/*
*	Sets the data in the uniform buffer.
*/
void VulkanUniformBuffer::UploadData(const void *RESTRICT newData) const NOEXCEPT
{
	void *mappedMemory;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, 0, _VniformBufferSize, 0, &mappedMemory));
	MemoryUtilities::CopyMemory(mappedMemory, newData, static_cast<uint64>(_VniformBufferSize));
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory);
}

/*
*	Returns the write descriptor set for this uniform buffer.
*/
VkWriteDescriptorSet VulkanUniformBuffer::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
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
void VulkanUniformBuffer::CreateDescriptorBufferInfo() NOEXCEPT
{
	_VulkanDescriptorBufferInfo.buffer = _VulkanBuffer;
	_VulkanDescriptorBufferInfo.offset = 0;
	_VulkanDescriptorBufferInfo.range = _VniformBufferSize;
}

/*
*	Creates the write descriptor set.
*/
void VulkanUniformBuffer::CreateWriteDescriptorSet() NOEXCEPT
{
	_VulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	_VulkanWriteDescriptorSet.pNext = nullptr;
	_VulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	_VulkanWriteDescriptorSet.dstBinding = 0;
	_VulkanWriteDescriptorSet.dstArrayElement = 0;
	_VulkanWriteDescriptorSet.descriptorCount = 1;
	_VulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	_VulkanWriteDescriptorSet.pImageInfo = nullptr;
	_VulkanWriteDescriptorSet.pBufferInfo = &_VulkanDescriptorBufferInfo;
	_VulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}