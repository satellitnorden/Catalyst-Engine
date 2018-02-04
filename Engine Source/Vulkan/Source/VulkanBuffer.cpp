//Header file.
#include <VulkanBuffer.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanBuffer::VulkanBuffer() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanBuffer::~VulkanBuffer() NOEXCEPT
{

}

/*
*	Initializes this Vulkan buffer.
*
*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
*	dataChunks - The number of data chunks that should be copied into the buffer.
*/
void VulkanBuffer::Initialize(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT
{
	//Calculate the total size of the buffer.
	VkDeviceSize bufferSize{ 0 };

	for (uint32 i = 0; i < dataChunks; ++i)
	{
		bufferSize += dataSizes[i];
	}

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingDeviceMemory);

	//Copy the data into the staging buffer.
	VkDeviceSize currentOffset{ 0 };

	for (uint32 i = 0; i < dataChunks; ++i)
	{
		void *mappedMemory;

		VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory, currentOffset, dataSizes[i], 0, &mappedMemory));
		MemoryUtilities::CopyMemory(mappedMemory, data[i], dataSizes[i]);
		vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory);

		currentOffset = dataSizes[i];
	}
	

	//Create the buffer.
	VulkanUtilities::CreateVulkanBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanBuffer, vulkanDeviceMemory);

	//Copy the staging buffer data to the buffer.
	VulkanUtilities::CopyBufferToBuffer(bufferSize, stagingBuffer, vulkanBuffer);

	//Free the staging device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory, nullptr);

	//Destroy the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);
}

/*
*	Releases this Vulkan vertex buffer.
*/
void VulkanBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, nullptr);
}