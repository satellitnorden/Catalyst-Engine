#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanDepthBuffer final : public VulkanImage
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VulkanDepthBuffer() NOEXCEPT
	{
		//Set the type.
		_Type = Type::VULKAN_DEPTH_BUFFER;
	}

	/*
	*	Initializes this Vulkan depth buffer.
	*/
	void Initialize(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT;

	/*
	*	Releases this Vulkan depth buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the sample count of this Vulkan depth buffer.
	*/
	const VkSampleCountFlagBits GetSampleCount() const NOEXCEPT { return _SampleCount; }

private:

	//The sample count.
	VkSampleCountFlagBits _SampleCount;

};
#endif