#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanRenderTarget final : public VulkanImage
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VulkanRenderTarget() NOEXCEPT
	{
		//Set the type.
		_Type = Type::VULKAN_RENDER_TARGET;
	}

	/*
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the sample count of the render target.
	*/
	FORCE_INLINE NO_DISCARD VkSampleCountFlagBits GetSampleCount() const NOEXCEPT { return _SampleCount; }

	/*
	*	Returns the Vulkan resolve image.
	*/
	const VkImage GetResolveImage() const NOEXCEPT { return _VulkanResolveImage; }

	/*
	*	Returns the Vulkan resolve image view.
	*/
	const VkImageView GetResolveImageView() const NOEXCEPT { return _VulkanResolveImageView; }

private:

	//The sample count.
	VkSampleCountFlagBits _SampleCount;

	//The Vulkan resolove image.
	VkImage _VulkanResolveImage{ VK_NULL_HANDLE };

	//The resolove allocation
	VmaAllocation _ResolveAllocation;

	//The Vulkan resolove image view.
	VkImageView _VulkanResolveImageView{ VK_NULL_HANDLE };

};
#endif