//Header file.
#include <Rendering/API/Vulkan/VulkanUniformBuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan uniform buffer.
*/
void VulkanUniformBuffer::Initialize(const VkDeviceSize newUniformBufferSize, const VkBufferUsageFlags usage) NOEXCEPT
{
	//Set the uniform buffer properties.
	_UniformBufferSize = newUniformBufferSize;

	//Create the Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(_UniformBufferSize, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _VulkanBuffer, _VulkanDeviceMemory);
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

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, 0, _UniformBufferSize, 0, &mappedMemory));
	MemoryUtilities::CopyMemory(mappedMemory, newData, static_cast<uint64>(_UniformBufferSize));
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory);
}