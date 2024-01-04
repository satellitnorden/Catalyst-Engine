#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanDepthBuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Initialize(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT
{
	//Find the most desirable depth buffer format.
	_VulkanFormat = format;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Set the sample count.
	_SampleCount = sample_count;

	//Create this Vulkan image.
	VkImageCreateInfo image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

	image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	image_info.format = _VulkanFormat;
	image_info.extent.width = extent.width;
	image_info.extent.height = extent.height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = _SampleCount;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VmaAllocationCreateInfo allocation_info = { };

	allocation_info.flags = 0;
	allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	VULKAN_ERROR_CHECK(vmaCreateImage(VULKAN_MEMORY_ALLOCATOR, &image_info, &allocation_info, &_VulkanImage, &_Allocation, nullptr));

	//Create the depth buffer image view!
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, _VulkanFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1, 1, _VulkanImageView);

	//Transition the image layout to a more appropriate layout.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);
}

/*
*	Releases this Vulkan depth buffer.
*/
void VulkanDepthBuffer::Release() NOEXCEPT
{
	//Destroy the Vulkan image view.
	ASSERT(_VulkanImageView, "Double deletion detected!");

	if (_VulkanImageView)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanImageView, nullptr);
		_VulkanImageView = VK_NULL_HANDLE;
	}

	//Destroy the Vulkan image.
	ASSERT(_VulkanImage, "Double deletion detected!");

	if (_VulkanImage)
	{
		vmaDestroyImage(VULKAN_MEMORY_ALLOCATOR, _VulkanImage, _Allocation);
		_VulkanImage = VK_NULL_HANDLE;
	}
}

/*
*	Finds and returns the most desirable format for the depth buffer.
*/
VkFormat VulkanDepthBuffer::FindMostDesirableDepthBufferFormat() const NOEXCEPT
{
	//Create a list of desirable formats, ranked from most desirable to least desirable.
	const DynamicArray<VkFormat> desirableFormats{ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };

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

#if defined(CATALYST_CONFIGURATION_DEBUG)
	//If a desirable format could not be retrieved, panic!
	BREAKPOINT();
#endif

	return VK_FORMAT_UNDEFINED;
}
#endif