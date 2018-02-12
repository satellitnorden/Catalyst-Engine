//Header file.
#include <VulkanCubeMapTexture.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanCubeMapTexture::VulkanCubeMapTexture() NOEXCEPT
{

}

/*
*	Default desctuctor.
*/
VulkanCubeMapTexture::~VulkanCubeMapTexture() NOEXCEPT
{

}

/*
*	Initializes this texture.
*/
void VulkanCubeMapTexture::Initialize(const uint32 width, const uint32 height, const byte *RESTRICT *RESTRICT textureData) NOEXCEPT
{
	//Calculate the image size and the side size.
	const VkDeviceSize imageSize = width * height * 4 * 6;
	const VkDeviceSize layerSize = imageSize / 6;

	//Set up the staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferDeviceMemory);

	void *RESTRICT data;
	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, 0, imageSize, 0, &data));

	//Copy the data into the staging buffer.
	for (uint8 i = 0; i < 6; ++i)
	{
		MemoryUtilities::CopyMemory(static_cast<byte *RESTRICT>(data) + (layerSize * i), textureData[i], static_cast<size_t>(layerSize));
	}

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, 6, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, vulkanImage, vulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 6, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, vulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, vulkanImage, 1, 6, width, height);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);

	//Clean up the staging buffer.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), stagingBuffer, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(vulkanImage, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1, 6, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler, TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::Repeat, 0.0f);

	//Create the descriptor image info.
	CreateDescriptorImageInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this texture.
*/
void VulkanCubeMapTexture::Release() NOEXCEPT
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
VkWriteDescriptorSet VulkanCubeMapTexture::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
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
void VulkanCubeMapTexture::CreateDescriptorImageInfo() NOEXCEPT
{
	vulkanDescriptorImageInfo.sampler = vulkanSampler;
	vulkanDescriptorImageInfo.imageView = vulkanImageView;
	vulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void VulkanCubeMapTexture::CreateWriteDescriptorSet() NOEXCEPT
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