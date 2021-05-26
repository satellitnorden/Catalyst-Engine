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

	//Calculate the image size.
	VkDeviceSize imageSize{ 0 };

	for (uint32 i{ 0 }; i < textureMipmapLevels; ++i)
	{
		imageSize += (textureWidth >> i) * (textureHeight >> i) * textureChannels * textureTexelSize;
	}

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	//Copy the data into the staging buffer.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, VK_WHOLE_SIZE, 0, &data));

	VkDeviceSize currentOffset{ 0 };

	for (uint8 i{ 0 }; i < textureMipmapLevels; ++i)
	{
		const VkDeviceSize mipSize{ (textureWidth >> i) * (textureHeight >> i) * textureChannels * static_cast<uint64>(textureTexelSize) };
		Memory::Copy(static_cast<byte*>(data) + currentOffset, textureData[i], mipSize);

		currentOffset += mipSize;
	}
	
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, VkImageType::VK_IMAGE_TYPE_2D, format, textureWidth, textureHeight, 1, textureMipmapLevels, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | image_usage_flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureMipmapLevels, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, _VulkanImage, textureMipmapLevels, 1, textureWidth, textureHeight, 1, textureChannels, sizeof(uint8));

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureMipmapLevels, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanImage);

	//Clean up the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, textureMipmapLevels, 1, _VulkanImageView);
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

	//Free the Vulkan device memory.
	ASSERT(_VulkanDeviceMemory, "Double deletion detected!");

	if (_VulkanDeviceMemory)
	{
		vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);
		_VulkanDeviceMemory = VK_NULL_HANDLE;
	}

	//Destroy the Vulkan image.
	ASSERT(_VulkanImage, "Double deletion detected!");

	if (_VulkanImage)
	{
		vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImage, nullptr);
		_VulkanImage = VK_NULL_HANDLE;
	}
}
#endif