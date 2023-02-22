#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanRenderTarget.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan render target.
*/
void VulkanRenderTarget::Initialize(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT
{
	//Set the extent
	_Extent = extent;

	//Set the sample count.
	_SampleCount = sample_count;

	//Set the format.
	_VulkanFormat = format;

	//Set the image layout.
	_VulkanImageLayout = VK_IMAGE_LAYOUT_GENERAL;

	//Create the Vulkan image.
	VkImageCreateInfo image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

	image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	image_info.format = format;
	image_info.extent.width = extent.width;
	image_info.extent.height = extent.height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = _SampleCount;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

	VmaAllocationCreateInfo allocation_info = { };

	allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	VULKAN_ERROR_CHECK(vmaCreateImage(VULKAN_MEMORY_ALLOCATOR, &image_info, &allocation_info, &_VulkanImage, &_Allocation, nullptr));

	//Transition the image to the correct layout.
	VulkanUtilities::TransitionImageToLayout(VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, _VulkanImage);

	//Create the image view.
	VulkanUtilities::CreateVulkanImageView(_VulkanImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, _VulkanImageView);

	//Create the resolve stuff.
	if (_SampleCount > VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
	{
		//Create the Vulkan resolve image.
		VkImageCreateInfo image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

		image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
		image_info.format = format;
		image_info.extent.width = extent.width;
		image_info.extent.height = extent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

		VmaAllocationCreateInfo allocation_info = { };

		allocation_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VULKAN_ERROR_CHECK(vmaCreateImage(VULKAN_MEMORY_ALLOCATOR, &image_info, &allocation_info, &_VulkanResolveImage, &_ResolveAllocation, nullptr));

		//Transition the resolve image to the correct layout.
		VulkanUtilities::TransitionImageToLayout(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, _VulkanResolveImage);

		//Create the resolve image view.
		VulkanUtilities::CreateVulkanImageView(_VulkanResolveImage, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, _VulkanResolveImageView);
	}
}

/*
*	Releases this Vulkan render target.
*/
void VulkanRenderTarget::Release() NOEXCEPT
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

	//Destroy the Vulkan resolve image view.
	ASSERT(_VulkanResolveImageView, "Double deletion detected!");

	if (_VulkanResolveImageView)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanResolveImageView, nullptr);
		_VulkanResolveImageView = VK_NULL_HANDLE;
	}

	//Destroy the Vulkan resolve image.
	ASSERT(_VulkanResolveImage, "Double deletion detected!");

	if (_VulkanResolveImage)
	{
		vmaDestroyImage(VULKAN_MEMORY_ALLOCATOR, _VulkanResolveImage, _ResolveAllocation);
		_VulkanResolveImage = VK_NULL_HANDLE;
	}
}
#endif