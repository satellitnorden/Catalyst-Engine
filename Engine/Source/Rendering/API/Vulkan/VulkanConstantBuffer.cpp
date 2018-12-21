#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/API/Vulkan/VulkanConstantBuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan buffer.
*
*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
*	dataChunks - The number of data chunks that should be copied into the buffer.
*/
void VulkanConstantBuffer::Initialize(const VkDeviceSize size) NOEXCEPT
{
	//Create this Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanBuffer, _VulkanDeviceMemory);
}

/*
*	Releases this Vulkan buffer.
*/
void VulkanConstantBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanBuffer, nullptr);
}

/*
*	Uploads data to this Vulkan buffer.
*/
void VulkanConstantBuffer::UploadData(const void *const RESTRICT *const RESTRICT data, const VkDeviceSize *const RESTRICT dataSizes, const uint32 dataChunks) NOEXCEPT
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

	void *mappedMemory;
	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory, 0, VK_WHOLE_SIZE, 0, &mappedMemory));

	for (uint32 i = 0; i < dataChunks; ++i)
	{
		MemoryUtilities::CopyMemory(static_cast<void*>(static_cast<byte*>(mappedMemory) + currentOffset), data[i], dataSizes[i]);

		currentOffset = dataSizes[i];
	}

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory);

	//Copy the staging buffer data to the buffer.
	VulkanUtilities::CopyBufferToBuffer(bufferSize, stagingBuffer, _VulkanBuffer);

	//Destroy the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

	//Free the staging device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory, nullptr);
}
#endif