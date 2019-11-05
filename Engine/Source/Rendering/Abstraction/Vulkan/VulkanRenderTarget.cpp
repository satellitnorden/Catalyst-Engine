#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanRenderTarget.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan render target.
*/
void VulkanRenderTarget::Initialize(const VkExtent2D extent, const VkFormat format) NOEXCEPT
{
	//Set the extent
	this->_Extent = extent;

	//Set the format.
	_VulkanFormat = format;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_GENERAL;

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(0, VkImageType::VK_IMAGE_TYPE_2D, format, extent.width, extent.height, 1, 1, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Transition the image to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanImage);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, _VulkanImageView);
}

/*
*	Releases this Vulkan render target.
*/
void VulkanRenderTarget::Release() NOEXCEPT
{
	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImage, nullptr);
}
#endif