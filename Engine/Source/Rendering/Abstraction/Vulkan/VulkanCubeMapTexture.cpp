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
void VulkanCubeMapTexture::Initialize(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT
{
	//Set the format.
	_VulkanFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Calculate the image size and the side size.
	const VkDeviceSize layerSize{ width * height * 4 * sizeof(float) };
	const VkDeviceSize imageSize{ layerSize * 6 };
	
	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	void *RESTRICT mappedData;
	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, imageSize, 0, (void**) &mappedData));

	//Copy the data into the staging buffer.
	Memory::Copy(mappedData, data, static_cast<uint64>(imageSize));

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VkImageType::VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT, width, height, 1, 1, 6, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 6, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, _VulkanImage, 1, 6, width, height, 1);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 6, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanImage);

	//Clean up the staging buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 6, _VulkanImageView);
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