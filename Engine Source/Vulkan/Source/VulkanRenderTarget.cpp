//Header file.
#include <VulkanRenderTarget.h>

//Vulkan.
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanRenderTarget::VulkanRenderTarget() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanRenderTarget::~VulkanRenderTarget() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan render target.
*/
void VulkanRenderTarget::Initialize(const uint32 width, const uint32 height) CATALYST_NOEXCEPT
{
	//Calculate the image size.
	const VkDeviceSize imageSize = width * height * 4;

	//Create the Vulkan image.
	VulkanUtilities::CreateVulkanImage(VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, vulkanImage, vulkanDeviceMemory);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, vulkanImage, vulkanImageView);

	//Create the Vulkan sampler.
	VulkanUtilities::CreateVulkanSampler(vulkanSampler);
}

/*
*	Releases this Vulkan render target.
*/
void VulkanRenderTarget::Release() CATALYST_NOEXCEPT
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
*	Prepares this render target for reading.
*/
void VulkanRenderTarget::PrepareForRead() CATALYST_NOEXCEPT
{
	//If the image is already in the correct layout, don't proceed.
	if (currentImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		return;

	//Transition the image to the correct layout.
	//VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, currentImageLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vulkanImage);

	//Set the current image layout.
	currentImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

/*
*	Prepares this render target for writing.
*/
void VulkanRenderTarget::PrepareForWrite() CATALYST_NOEXCEPT
{
	//If the image is already in the correct layout, don't proceed.
	if (currentImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		return;

	//Transition the image to the correct layout.
	//VulkanUtilities::TransitionImageToLayout(VK_FORMAT_R8G8B8A8_UNORM, currentImageLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vulkanImage);

	//Set the current image layout.
	currentImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}