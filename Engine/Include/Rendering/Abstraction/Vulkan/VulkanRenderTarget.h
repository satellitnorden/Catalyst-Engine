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
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the extent of the render target.
	*/
	FORCE_INLINE NO_DISCARD VkExtent2D GetExtent() const NOEXCEPT { return _Extent; }

	/*
	*	Returns the sample count of the render target.
	*/
	FORCE_INLINE NO_DISCARD VkSampleCountFlagBits GetSampleCount() const NOEXCEPT { return _SampleCount; }

private:

	//The extent of the image.
	VkExtent2D _Extent;

	//The sample count.
	VkSampleCountFlagBits _SampleCount;

};
#endif