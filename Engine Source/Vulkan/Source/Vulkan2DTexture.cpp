//Header file.
#include <Vulkan2DTexture.h>

//Graphics.
#include <TextureData.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
Vulkan2DTexture::Vulkan2DTexture() NOEXCEPT
{

}

/*
*	Default desctuctor.
*/
Vulkan2DTexture::~Vulkan2DTexture() NOEXCEPT
{

}

/*
*	Initializes this Vulkan 2D texture.
*/
void Vulkan2DTexture::Initialize(const TextureData &textureData) NOEXCEPT
{
	//Calculate the image size.
	VkDeviceSize imageSize{ 0 };
	const size_t textureDataSize{ textureData.textureDataContainer.textureData.Size() };

	for (size_t i = 0; i < textureDataSize; ++i)
	{
		imageSize += (textureData.textureDataContainer.textureWidth >> i) * (textureData.textureDataContainer.textureHeight >> i) * textureData.textureDataContainer.textureChannels * textureData.textureDataContainer.textureTexelSize;
	}

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	//Copy the data into the staging buffer.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, VK_WHOLE_SIZE, 0, &data));

	size_t currentOffset{ 0 };

	for (size_t i = 0; i < textureDataSize; ++i)
	{
		const size_t mipSize{ (textureData.textureDataContainer.textureWidth >> i) * (textureData.textureDataContainer.textureHeight >> i) * textureData.textureDataContainer.textureChannels * static_cast<size_t>(textureData.textureDataContainer.textureTexelSize) };
		MemoryUtilities::CopyMemory(static_cast<byte*>(data) + currentOffset, textureData.textureDataContainer.textureData[i], mipSize);

		currentOffset += mipSize;
	}
	
	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Deduce the format of the images.
	VkFormat format{ VulkanUtilities::GetVulkanFormat(textureData.textureFormat) };

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, format, textureData.textureDataContainer.textureWidth, textureData.textureDataContainer.textureHeight, static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, vulkanImage, vulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(format, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, vulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, vulkanImage, static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), 1, textureData.textureDataContainer.textureWidth, textureData.textureDataContainer.textureHeight);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(format, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);

	//Clean up the staging buffer.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(vulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), 1, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler, textureData.magnificationFilter, textureData.mipmapMode, textureData.addressMode, static_cast<float>(textureData.textureDataContainer.textureData.Size()));

	//Create the descriptor image info.
	CreateDescriptorImageInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan 2D texture.
*/
void Vulkan2DTexture::Release() NOEXCEPT
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
VkWriteDescriptorSet Vulkan2DTexture::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
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
void Vulkan2DTexture::CreateDescriptorImageInfo() NOEXCEPT
{
	vulkanDescriptorImageInfo.sampler = vulkanSampler;
	vulkanDescriptorImageInfo.imageView = vulkanImageView;
	vulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void Vulkan2DTexture::CreateWriteDescriptorSet() NOEXCEPT
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