//Header file.
#include <VulkanTexture.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanTexture::VulkanTexture() CATALYST_NOEXCEPT
{

}

/*
*	Default desctuctor.
*/
VulkanTexture::~VulkanTexture() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan texture.
*/
void VulkanTexture::Initialize(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT
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

	vkMapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBufferDeviceMemory, 0, imageSize, 0, &data);
	MemoryUtilities::CopyMemory(data, textureData, static_cast<size_t>(imageSize));
	vkUnmapMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBufferDeviceMemory);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, vulkanImage, vulkanDeviceMemory);

	//Transition the Vulkan image to the correct layout for writing.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vulkanImage);

	//Copy the buffer to the Vulkan image.
	VulkanUtilities::CopyBufferToImage(stagingBuffer, vulkanImage, width, height);

	//Transition the Vulkan image to the correct layout for reading.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vulkanImage);

	//Clean up the staging buffer.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBufferDeviceMemory, nullptr);
	vkDestroyBuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), stagingBuffer, nullptr);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, vulkanImage, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler);

	//Create the descriptor image info.
	CreateDescriptorImageInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan texture.
*/
void VulkanTexture::Release() CATALYST_NOEXCEPT
{
	//Destroy Vulkan sampler.
	vkDestroySampler(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSampler, nullptr);

	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanImageView, nullptr);

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanImage, nullptr);
}

/*
*	Returns the write descriptor set for this uniform buffer.
*/
VkWriteDescriptorSet VulkanTexture::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT
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
void VulkanTexture::CreateDescriptorImageInfo() CATALYST_NOEXCEPT
{
	vulkanDescriptorImageInfo.sampler = vulkanSampler;
	vulkanDescriptorImageInfo.imageView = vulkanImageView;
	vulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void VulkanTexture::CreateWriteDescriptorSet() CATALYST_NOEXCEPT
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