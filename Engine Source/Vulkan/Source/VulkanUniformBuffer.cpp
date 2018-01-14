//Header file.
#include <VulkanUniformBuffer.h>

//Vulkan.
#include <VulkanDescriptorPool.h>
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanUniformBuffer::VulkanUniformBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanUniformBuffer::~VulkanUniformBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan uniform buffer.
*/
void VulkanUniformBuffer::Initialize(const VkDeviceSize newUniformBufferSize) CATALYST_NOEXCEPT
{
	//Set the uniform buffer properties.
	uniformBufferSize = newUniformBufferSize;

	//Create the Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanBuffer, vulkanDeviceMemory);

	//Create the descriptor buffer info.
	CreateDescriptorBufferInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan uniform buffer.
*/
void VulkanUniformBuffer::Release() CATALYST_NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanBuffer, nullptr);
}

/*
*	Sets the data in the uniform buffer.
*/
void VulkanUniformBuffer::UploadData(void *CATALYST_RESTRICT newData) const CATALYST_NOEXCEPT
{
	void *mappedMemory;

	vkMapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory, 0, uniformBufferSize, 0, &mappedMemory);
	MemoryUtilities::CopyMemory(mappedMemory, newData, static_cast<size_t>(uniformBufferSize));
	vkUnmapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory);
}

/*
*	Returns the write descriptor set for this uniform buffer.
*/
VkWriteDescriptorSet VulkanUniformBuffer::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT
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
void VulkanUniformBuffer::CreateDescriptorBufferInfo() CATALYST_NOEXCEPT
{
	vulkanDescriptorBufferInfo.buffer = vulkanBuffer;
	vulkanDescriptorBufferInfo.offset = 0;
	vulkanDescriptorBufferInfo.range = uniformBufferSize;
}

/*
*	Creates the write descriptor set.
*/
void VulkanUniformBuffer::CreateWriteDescriptorSet() CATALYST_NOEXCEPT
{
	vulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vulkanWriteDescriptorSet.pNext = nullptr;
	vulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	vulkanWriteDescriptorSet.dstBinding = 0;
	vulkanWriteDescriptorSet.dstArrayElement = 0;
	vulkanWriteDescriptorSet.descriptorCount = 1;
	vulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vulkanWriteDescriptorSet.pImageInfo = nullptr;
	vulkanWriteDescriptorSet.pBufferInfo = &vulkanDescriptorBufferInfo;
	vulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}