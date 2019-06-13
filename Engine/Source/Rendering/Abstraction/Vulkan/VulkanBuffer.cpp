#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan buffer.
*/
void VulkanBuffer::Initialize(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT
{
	//Set the memory properties.
	_MemoryProperties = memoryProperties;

	//If the buffer is not created with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, then we need to specify that this buffer will be used as a transfer destination.
	const VkBufferUsageFlags checkedUsage{ !TEST_BIT(_MemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage : usage };

	//Create this Vulkan buffer.
	VulkanUtilities::CreateVulkanBuffer(size, checkedUsage, memoryProperties, _VulkanBuffer, _VulkanDeviceMemory);
}

/*
*	Releases this Vulkan buffer.
*/
void VulkanBuffer::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan vertex buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanBuffer, nullptr);
}

/*
*	Uploads data to this Vulkan buffer.
*
*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
*	dataChunks - The number of data chunks that should be copied into the buffer.
*/
void VulkanBuffer::UploadData(const void *const RESTRICT *const RESTRICT data, const VkDeviceSize *const RESTRICT dataSizes, const uint32 dataChunks, VulkanCommandBuffer *const RESTRICT commandBuffer) NOEXCEPT
{
	//If this buffer was created with the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, the memory can be mapped directly and copied over, otherwise a staging buffer needs to be created.
	if (TEST_BIT(_MemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		//Copy the data into the buffer.
		VkDeviceSize currentOffset{ 0 };

		void *mappedMemory;
		VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, 0, VK_WHOLE_SIZE, 0, &mappedMemory));

		for (uint32 i = 0; i < dataChunks; ++i)
		{
			Memory::CopyMemory(static_cast<void*>(static_cast<byte*>(mappedMemory) + currentOffset), data[i], dataSizes[i]);

			currentOffset = dataSizes[i];
		}

		vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory);
	}

	else
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
			Memory::CopyMemory(static_cast<void*>(static_cast<byte*>(mappedMemory) + currentOffset), data[i], dataSizes[i]);

			currentOffset = dataSizes[i];
		}

		vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory);

		//Copy the staging buffer data to the buffer.
		VulkanUtilities::CopyBufferToBuffer(bufferSize, stagingBuffer, _VulkanBuffer, commandBuffer);

		//Destroy the staging buffer.
		vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

		//Free the staging device memory.
		vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingDeviceMemory, nullptr);
	}
}
#endif