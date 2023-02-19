#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/Vulkan2DTexture.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan 2D texture with void data.
*/
void Vulkan2DTexture::Initialize(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkImageUsageFlags image_usage_flags) NOEXCEPT
{
	//Set the format.
	_VulkanFormat = format;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Create the command pool.
	static thread_local VulkanCommandPool *const RESTRICT command_pool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

	//Create the command buffer.
	VulkanCommandBuffer command_buffer;
	command_pool->AllocatePrimaryCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Create this Vulkan image.
	VkImageCreateInfo image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

	image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	image_info.format = format;
	image_info.extent.width = textureWidth;
	image_info.extent.height = textureHeight;
	image_info.extent.depth = 1;
	image_info.mipLevels = textureMipmapLevels;
	image_info.arrayLayers = 1;
	image_info.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | image_usage_flags;

	VmaAllocationCreateInfo allocation_info = { };

	allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	VULKAN_ERROR_CHECK(vmaCreateImage(VULKAN_MEMORY_ALLOCATOR, &image_info, &allocation_info, &_VulkanImage, &_Allocation, nullptr));

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(&command_buffer, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureMipmapLevels, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);

	//Calculate the image size.
	VkDeviceSize image_size{ 0 };

	for (uint32 i{ 0 }; i < textureMipmapLevels; ++i)
	{
		image_size += (textureWidth >> i) * (textureHeight >> i) * textureChannels * textureTexelSize;
	}

	//Set up the staging buffer.
	VkBuffer staging_buffer;
	VmaAllocation staging_allocation;

	//Create the staging buffer.
	{
		VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = image_size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocation_info = { };
		allocation_info.flags |= VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO;

		VULKAN_ERROR_CHECK(vmaCreateBuffer(VULKAN_MEMORY_ALLOCATOR, &buffer_info, &allocation_info, &staging_buffer, &staging_allocation, nullptr));
	}

	//Copy the data into the staging buffer.
	void* mapped_memory;
	VULKAN_ERROR_CHECK(vmaMapMemory(VULKAN_MEMORY_ALLOCATOR, staging_allocation, &mapped_memory));

	VkDeviceSize current_offset{ 0 };

	for (uint8 i{ 0 }; i < textureMipmapLevels; ++i)
	{
		const VkDeviceSize mip_size{ (textureWidth >> i) * (textureHeight >> i) * textureChannels * static_cast<uint64>(textureTexelSize) };
		Memory::Copy(static_cast<byte*>(mapped_memory) + current_offset, textureData[i], mip_size);

		current_offset += mip_size;
	}

	vmaUnmapMemory(VULKAN_MEMORY_ALLOCATOR, staging_allocation);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(&command_buffer, staging_buffer, _VulkanImage, textureMipmapLevels, 1, textureWidth, textureHeight, 1, textureChannels, sizeof(uint8));

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(&command_buffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureMipmapLevels, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanImage);

	//End the transfer command buffer.
	command_buffer.End();

	//Submit the command buffer.
	VulkanFence fence;
	fence.Initialize(0);
	VulkanInterface::Instance->GetTransferQueue()->Submit(command_buffer, 0, nullptr, 0, 0, nullptr, fence.Get());

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, textureMipmapLevels, 1, _VulkanImageView);

	//Wait for the transfer command to finish.
	fence.WaitFor();

	//Release the fence.
	fence.Release();

	//Free the transfer command buffer,
	command_pool->FreeCommandBuffer(command_buffer);

	//Destroy the staging buffer.
	vmaDestroyBuffer(VULKAN_MEMORY_ALLOCATOR, staging_buffer, staging_allocation);
}

/*
*	Releases this Vulkan 2D texture.
*/
void Vulkan2DTexture::Release() NOEXCEPT
{
	//Destroy the Vulkan image view.
	ASSERT(_VulkanImageView, "Double deletion detected!");

	if (_VulkanImageView)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);
		_VulkanImageView = VK_NULL_HANDLE;
	}

	//Destroy the Vulkan image.
	ASSERT(_VulkanImage, "Double deletion detected!");

	if (_VulkanImage)
	{
		vmaDestroyImage(VULKAN_MEMORY_ALLOCATOR, _VulkanImage, _Allocation);
		_VulkanImage = VK_NULL_HANDLE;
	}
}
#endif