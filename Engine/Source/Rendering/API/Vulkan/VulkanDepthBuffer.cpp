//Header file.
#include <Rendering/API/Vulkan/VulkanDepthBuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Initialize(const VkExtent2D imageExtent) NOEXCEPT
{
	//Find the most desirable depth buffer format.
	_Format = FindMostDesirableDepthBufferFormat();

	//Create the depth buffer image!
	VulkanUtilities::CreateVulkanImage(0, _Format, imageExtent.width, imageExtent.height, 1, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VulkanImage, _VulkanDeviceMemory);

	//Create the depth buffer image view!
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, _Format, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1, 1, _VulkanImageView);

	//Transition the image layout to a more appropriate layout.
	VulkanUtilities::TransitionImageToLayout(0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, _VulkanImage);
}

/*
*	Releases this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Release() NOEXCEPT
{
	//Destroy the Vulkan image.
	vkDestroyImage(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImage, nullptr);

	//Free the device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the Vulkan image view.
	vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);
}

/*
*	Finds and returns the most desirable format for the depth buffer.
*/
VkFormat VulkanDepthBuffer::FindMostDesirableDepthBufferFormat() const NOEXCEPT
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
	BREAKPOINT();
#endif

	return VK_FORMAT_UNDEFINED;
}