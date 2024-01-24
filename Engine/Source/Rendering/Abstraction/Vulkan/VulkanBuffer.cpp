#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan buffer.
*/
void VulkanBuffer::Initialize(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memory_properties) NOEXCEPT
{
	//Set the allocated size.
	_AllocatedSize = size;

	//Set the memory properties.
	_MemoryProperties = memory_properties;

	//If the buffer is not created with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, then we need to specify that this buffer will be used as a transfer destination.
	const VkBufferUsageFlags checked_usage{ !TEST_BIT(_MemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage : usage };

	//Create this Vulkan buffer.
	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size;
	buffer_info.usage = checked_usage;

	VmaAllocationCreateInfo allocation_info = { };

	if (TEST_BIT(_MemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		allocation_info.flags |= VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	}

	else
	{
		allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	}
	
	VULKAN_ERROR_CHECK(vmaCreateBuffer(VULKAN_MEMORY_ALLOCATOR, &buffer_info, &allocation_info, &_VulkanBuffer, &_Allocation, nullptr));
}

/*
*	Releases this Vulkan buffer.
*/
void VulkanBuffer::Release() NOEXCEPT
{
	//Destroy this Vulkan buffer.
	vmaDestroyBuffer(VULKAN_MEMORY_ALLOCATOR, _VulkanBuffer, _Allocation);
}

/*
*	Uploads data to this Vulkan buffer.
*
*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
*	dataChunks - The number of data chunks that should be copied into the buffer.
*/
void VulkanBuffer::UploadData(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint32 data_chunks) NOEXCEPT
{
	//Count the total size being uploaded.
	uint64 total_size{ 0 };

	for (uint32 i{ 0 }; i < data_chunks; ++i)
	{
		total_size += data_sizes[i];
	}

	//If this buffer was created with the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, the memory can be mapped directly and copied over, otherwise a staging buffer needs to be created.
	if (TEST_BIT(_MemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		//Copy the data into the buffer.
		VkDeviceSize current_offset{ 0 };

		void *mapped_memory;
		VULKAN_ERROR_CHECK(vmaMapMemory(VULKAN_MEMORY_ALLOCATOR, _Allocation, &mapped_memory));

		for (uint32 i = 0; i < data_chunks; ++i)
		{
			Memory::Copy(static_cast<void*>(static_cast<byte*>(mapped_memory) + current_offset), data[i], data_sizes[i]);

			current_offset = data_sizes[i];
		}

		vmaUnmapMemory(VULKAN_MEMORY_ALLOCATOR, _Allocation);
	}

	else
	{
		//Calculate the total size of the buffer.
		VkDeviceSize buffer_size{ 0 };

		for (uint32 i = 0; i < data_chunks; ++i)
		{
			buffer_size += data_sizes[i];
		}

		//Set up the staging buffer.
		VkBuffer staging_buffer;
		VmaAllocation staging_allocation;

		//Create the staging buffer.
		{
			VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			buffer_info.size = buffer_size;
			buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			VmaAllocationCreateInfo allocation_info = { };
			allocation_info.flags |= VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO;

			VULKAN_ERROR_CHECK(vmaCreateBuffer(VULKAN_MEMORY_ALLOCATOR, &buffer_info, &allocation_info, &staging_buffer, &staging_allocation, nullptr));
		}

		//Copy the data into the buffer.
		VkDeviceSize current_offset{ 0 };

		void* mapped_memory;
		VULKAN_ERROR_CHECK(vmaMapMemory(VULKAN_MEMORY_ALLOCATOR, staging_allocation, &mapped_memory));

		for (uint32 i = 0; i < data_chunks; ++i)
		{
			Memory::Copy(static_cast<void*>(static_cast<byte*>(mapped_memory) + current_offset), data[i], data_sizes[i]);

			current_offset = data_sizes[i];
		}

		vmaUnmapMemory(VULKAN_MEMORY_ALLOCATOR, staging_allocation);

		//Copy the staging buffer data to the buffer.
		VulkanUtilities::CopyBufferToBuffer(buffer_size, staging_buffer, _VulkanBuffer);

		//Destroy the staging buffer.
		vmaDestroyBuffer(VULKAN_MEMORY_ALLOCATOR, staging_buffer, staging_allocation);
	}
}
#endif