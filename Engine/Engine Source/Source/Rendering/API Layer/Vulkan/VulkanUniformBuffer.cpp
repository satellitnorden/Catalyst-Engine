//Header file.
#include <Rendering/API Layer/Vulkan/VulkanUniformBuffer.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

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
void VulkanUniformBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, nullptr);
}

/*
*	Sets the data in the uniform buffer.
*/
void VulkanUniformBuffer::UploadData(void *RESTRICT newData) const NOEXCEPT
{
	void *mappedMemory;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, 0, uniformBufferSize, 0, &mappedMemory));
	MemoryUtilities::CopyMemory(mappedMemory, newData, StaticCast<uint64>(uniformBufferSize));
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory);
}

/*
*	Returns the write descriptor set for this uniform buffer.
*/
VkWriteDescriptorSet VulkanUniformBuffer::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
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
void VulkanUniformBuffer::CreateDescriptorBufferInfo() NOEXCEPT
{
	vulkanDescriptorBufferInfo.buffer = vulkanBuffer;
	vulkanDescriptorBufferInfo.offset = 0;
	vulkanDescriptorBufferInfo.range = uniformBufferSize;
}

/*
*	Creates the write descriptor set.
*/
void VulkanUniformBuffer::CreateWriteDescriptorSet() NOEXCEPT
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