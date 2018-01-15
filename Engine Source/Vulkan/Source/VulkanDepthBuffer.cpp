//Header file.
#include <VulkanDepthBuffer.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanDepthBuffer::VulkanDepthBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanDepthBuffer::~VulkanDepthBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Initialize(const VkExtent2D imageExtent) CATALYST_NOEXCEPT
{
	//Find the most desirable depth buffer format.
	format = FindMostDesirableDepthBufferFormat();

	//Create the depth buffer image!
	VulkanUtilities::CreateVulkanImage(format, imageExtent.width, imageExtent.height, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, vulkanImage, vulkanDeviceMemory);

	//Create the depth buffer image view!
	VulkanUtilities::CreateVulkanImageView(format, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanImage, vulkanImageView);

	//Transition the image layout to a more appropriate layout.
	VulkanUtilities::TransitionImageToLayout(format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vulkanImage);
}

/*
*	Releases this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImage, nullptr);

	//Free the device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImageView, nullptr);
}

/*
*	Finds and returns the most desirable format for the depth buffer.
*/
VkFormat VulkanDepthBuffer::FindMostDesirableDepthBufferFormat() const CATALYST_NOEXCEPT
{
	//Create a list of desirable formats, ranked from most desirable to least desirable.
	const DynamicArray<VkFormat> desirableFormats{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };

	for (auto desirableFormat : desirableFormats)
	{
		//Get the physical device format properties for this format.
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(VulkanInterface::Instance->GetPhysicalDevice().Get(), desirableFormat, &formatProperties);

		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return desirableFormat;
		}
	}

#if !defined(CATALYST_FINAL)
	//If a desirable format could not be retrieved, panic!
	BREAKPOINT;
#endif

	return VK_FORMAT_UNDEFINED;
}