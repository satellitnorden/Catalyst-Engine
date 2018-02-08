//Header file.
#include <Vulkan2DTexture.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
Vulkan2DTexture::Vulkan2DTexture() CATALYST_NOEXCEPT
{

}

/*
*	Default desctuctor.
*/
Vulkan2DTexture::~Vulkan2DTexture() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan 2D texture.
*/
void Vulkan2DTexture::Initialize(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT
{
	//Calculate the image size.
	const VkDeviceSize imageSize = width * height * 4;

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	//Copy the data into the staging buffer.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, imageSize, 0, &data));
	MemoryUtilities::CopyMemory(data, textureData, static_cast<size_t>(imageSize));
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, vulkanImage, vulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, vulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, vulkanImage, 1, width, height);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);

	//Clean up the staging buffer.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(vulkanImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler);

	//Create the descriptor image info.
	CreateDescriptorImageInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan 2D texture.
*/
void Vulkan2DTexture::Release() CATALYST_NOEXCEPT
{
	//Destroy Vulkan sampler.
	vkDestroySampler(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSampler, nullptr);

	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImageView, nullptr);

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImage, nullptr);
}

/*
*	Returns the write descriptor set for this texture.
*/
VkWriteDescriptorSet Vulkan2DTexture::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT
{
	VkWriteDescriptorSet vulkanWriteDescriptorSetCopy{ vulkanWriteDescriptorSet };

	vulkanWriteDescriptorSetCopy.dstSet = vulkanDescriptorSet.Get();
	vulkanWriteDescriptorSetCopy.dstBinding = binding;
	vulkanWriteDescriptorSetCopy.pImageInfo = &vulkanDescriptorImageInfo;

	return vulkanWriteDescriptorSetCopy;
}

/*
*	Creates the descriptor image info.
*/
void Vulkan2DTexture::CreateDescriptorImageInfo() CATALYST_NOEXCEPT
{
	vulkanDescriptorImageInfo.sampler = vulkanSampler;
	vulkanDescriptorImageInfo.imageView = vulkanImageView;
	vulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void Vulkan2DTexture::CreateWriteDescriptorSet() CATALYST_NOEXCEPT
{
	vulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vulkanWriteDescriptorSet.pNext = nullptr;
	vulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	vulkanWriteDescriptorSet.dstBinding = 0;
	vulkanWriteDescriptorSet.dstArrayElement = 0;
	vulkanWriteDescriptorSet.descriptorCount = 1;
	vulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vulkanWriteDescriptorSet.pImageInfo = &vulkanDescriptorImageInfo;
	vulkanWriteDescriptorSet.pBufferInfo = nullptr;
	vulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}