//Header file.
#include <Rendering/API/Vulkan/VulkanRenderTarget.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan render target.
*/
void VulkanRenderTarget::Initialize(const VkExtent2D extent, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	//Set the extent
	this->_Extent = extent;

	//Set the format.
	this->_Format = format;

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, format, extent.width, extent.height, 1, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Transition the image to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanImage);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, _VulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(_VulkanSampler, magnificationFilter, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode, 0.0f);

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
	vkDestroySampler(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSampler, nullptr);

	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImage, nullptr);
}

/*
*	Returns the write descriptor set for this texture.
*/
VkWriteDescriptorSet VulkanRenderTarget::GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT
{
	VkWriteDescriptorSet vulkanWriteDescriptorSetCopy{ _VulkanWriteDescriptorSet };

	vulkanWriteDescriptorSetCopy.dstSet = vulkanDescriptorSet.Get();
	vulkanWriteDescriptorSetCopy.dstBinding = binding;
	vulkanWriteDescriptorSetCopy.pImageInfo = &_VulkanDescriptorImageInfo;

	return vulkanWriteDescriptorSetCopy;
}

/*
*	Creates the descriptor image info.
*/
void VulkanRenderTarget::CreateDescriptorImageInfo() NOEXCEPT
{
	_VulkanDescriptorImageInfo.sampler = _VulkanSampler;
	_VulkanDescriptorImageInfo.imageView = _VulkanImageView;
	_VulkanDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

/*
*	Creates the write descriptor set.
*/
void VulkanRenderTarget::CreateWriteDescriptorSet() NOEXCEPT
{
	_VulkanWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	_VulkanWriteDescriptorSet.pNext = nullptr;
	_VulkanWriteDescriptorSet.dstSet = VK_NULL_HANDLE;
	_VulkanWriteDescriptorSet.dstBinding = 0;
	_VulkanWriteDescriptorSet.dstArrayElement = 0;
	_VulkanWriteDescriptorSet.descriptorCount = 1;
	_VulkanWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	_VulkanWriteDescriptorSet.pImageInfo = &_VulkanDescriptorImageInfo;
	_VulkanWriteDescriptorSet.pBufferInfo = nullptr;
	_VulkanWriteDescriptorSet.pTexelBufferView = nullptr;
}