#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanCubeMapTexture.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this texture.
*/
void VulkanCubeMapTexture::Initialize(const DynamicArray<DynamicArray<float32>> &data, const uint32 width, const uint32 height) NOEXCEPT
{
	//Set the format.
	_VulkanFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_GENERAL;

	//Set the extent.
	_Extent = VkExtent3D(width, height, 1);

	//Calculate the image size.
	VkDeviceSize image_size{ 0 };

	for (uint64 i{ 0 }; i < data.Size(); ++i)
	{
		image_size += (width >> i) * (height >> i) * 4 * sizeof(float32) * 6;
	}

	//Set up the staging buffer.
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_device_memory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_device_memory);

	//Copy the data into the staging buffer.
	void *mapped_data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), staging_buffer_device_memory, 0, VK_WHOLE_SIZE, 0, &mapped_data));

	VkDeviceSize current_offset{ 0 };

	for (uint64 i{ 0 }; i < data.Size(); ++i)
	{
		const VkDeviceSize mip_size{ (width >> i) * (height >> i) * 4 * sizeof(float32) * 6 };
		Memory::Copy(static_cast<byte*>(mapped_data) + current_offset, data[i].Data(), mip_size);

		current_offset += mip_size;
	}

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), staging_buffer_device_memory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VkImageType::VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT, width, height, 1, static_cast<uint32>(data.Size()), 6, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(data.Size()), 6, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(staging_buffer, _VulkanImage, static_cast<uint32>(data.Size()), 6, width, height, 1, 4, sizeof(float32), 1);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_NONE, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, static_cast<uint32>(data.Size()), 6, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, _VulkanImage);

	//Clean up the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), staging_buffer, nullptr);
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), staging_buffer_device_memory, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, static_cast<uint32>(data.Size()), 6, _VulkanImageView);
}

/*
*	Releases this texture.
*/
void VulkanCubeMapTexture::Release() NOEXCEPT
{
	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImage, nullptr);
}
#endif