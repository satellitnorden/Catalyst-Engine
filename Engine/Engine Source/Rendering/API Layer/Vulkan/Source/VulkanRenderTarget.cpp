//Header file.
#include <VulkanRenderTarget.h>

//Vulkan.
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanRenderTarget::VulkanRenderTarget() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanRenderTarget::~VulkanRenderTarget() NOEXCEPT
{

}

/*
*	Initializes this Vulkan render target.
*/
void VulkanRenderTarget::Initialize(const VkExtent2D extent) NOEXCEPT
{
	//Calculate the image size.
	imageSize = extent.width * extent.height * SizeOf(float);

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, VK_FORMAT_R32G32B32A32_SFLOAT, extent.width, extent.height, 1, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, vulkanImage, vulkanDeviceMemory);

	//Transition the image to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(vulkanImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f);

	//Create the descriptor image info.
	CreateDescriptorImageInfo();

	//Create the write descriptor set.
	CreateWriteDescriptorSet();
}

/*
*	Releases this Vulkan render target.
*/
void VulkanRenderTarget::Release() NOEXCEPT
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
VkWriteDescriptorSet VulkanRenderTarget::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
{
	VkWriteDescriptorSet vulkanWriteDescriptorSetCopy{ vulkanWriteDescriptorSet };

	vulkanWriteDescriptorSetCopy.dstSet = vulkanDescriptorSet.Get();
	vulkanWriteDescriptorSetCopy.dstBinding = binding;
	vulkanWriteDescriptorSetCopy.pImageInfo = &vulkanDescriptorImageInfo;

	return vulkanWriteDescriptorSetCopy;
}

/*
*	Returns the data contained in the image.
*/
void VulkanRenderTarget::GetImageData(DynamicArray<float> &imageData) NOEXCEPT
{
	//Map the memory.
	void *data;

	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, 0, VK_WHOLE_SIZE, 0, &data));

	//Copy the contents of the image to the image data.
	imageData.Reserve(imageSize);
	MemoryUtilities::CopyMemory(imageData.Data(), data, imageSize);

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory);

	/*
	//Create an intermediate buffer to copy the data to.
	VkBuffer intermediateBuffer;
	VkDeviceMemory intermediateBufferDeviceMemory;

	//Create the staging buffer.
	VulkanUtilities::CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, intermediateBuffer, intermediateBufferDeviceMemory);

	//Transition the image to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);

	//Copy the image to the buffer.
	VulkanUtilities::CopyIma

	//Transition the image back to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R32G32B32A32_SFLOAT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, vulkanImage);
	*/
}

/*
*	Creates the descriptor image info.
*/
void VulkanRenderTarget::CreateDescriptorImageInfo() NOEXCEPT
{
	vulkanDescriptorImageInfo.sampler = vulkanSampler;
	vulkanDescriptorImageInfo.imageView = vulkanImageView;
	vulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void VulkanRenderTarget::CreateWriteDescriptorSet() NOEXCEPT
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