//Header file.
#include <VulkanVertexBuffer.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanVertexBuffer::VulkanVertexBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanVertexBuffer::~VulkanVertexBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan vertex buffer.
*/
void VulkanVertexBuffer::Initialize(const DynamicArray<Vertex> &vertices) CATALYST_NOEXCEPT
{
	//Calculate the size of the vertex buffer.
	const VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.Size();

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingDeviceMemory);

	//Copy the vertices data into the staging buffer.
	void *data;

	vkMapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory, 0, vertexBufferSize, 0, &data);
	MemoryUtilities::CopyMemory(data, vertices.Data(), static_cast<size_t>(vertexBufferSize));
	vkUnmapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory);

	//Create the vertex buffer.
	VulkanUtilities::CreateVulkanBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanBuffer, vulkanDeviceMemory);

	//Copy the staging buffer data to the vertex buffer.
	VulkanUtilities::CopyBufferToBuffer(vertexBufferSize, stagingBuffer, vulkanBuffer);

	//Free the staging device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingDeviceMemory, nullptr);

	//Destroy the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBuffer, nullptr);
}

/*
*	Releases this Vulkan vertex buffer.
*/
void VulkanVertexBuffer::Release() CATALYST_NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanBuffer, nullptr);
}