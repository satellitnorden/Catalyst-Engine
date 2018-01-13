//Header file.
#include <VulkanIndexBuffer.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanIndexBuffer::VulkanIndexBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanIndexBuffer::~VulkanIndexBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan vertex buffer.
*/
void VulkanIndexBuffer::Initialize(const DynamicArray<uint32> &indices) CATALYST_NOEXCEPT
{
	//Calculate the size of the vertex buffer.
	const VkDeviceSize indexBufferSize = sizeof(uint32) * indices.Size();

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingDeviceMemory);

	//Copy the vertices data into the staging buffer.
	void *data;

	vkMapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, indices.Data(), indexBufferSize);
	vkUnmapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory);

	//Create the vertex buffer.
	VulkanUtilities::CreateVulkanBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanBuffer, vulkanDeviceMemory);

	//Copy the staging buffer data to the vertex buffer.
	VulkanUtilities::CopyBufferToBuffer(indexBufferSize, stagingBuffer, vulkanBuffer);

	//Free the staging device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory, nullptr);

	//Destroy the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBuffer, nullptr);
}

/*
*	Releases this Vulkan vertex buffer.
*/
void VulkanIndexBuffer::Release() CATALYST_NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanBuffer, nullptr);
}