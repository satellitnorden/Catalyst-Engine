#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanDepthBuffer final
{

public:

	/*
	*	Initializes this Vulkan depth buffer.
	*/
	void Initialize(const VkExtent2D imageExtent) NOEXCEPT;

	/*
	*	Releases this Vulkan depth buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the image view for this Vulkan depth buffer.
	*/
	const VkImageView& GetImageView() const NOEXCEPT { return _VulkanImageView; }

	/*
	*	Returns the format of this Vulkan depth buffer.
	*/
	const VkFormat& GetFormat() const NOEXCEPT { return _Format; }

private:

	//The Vulkan image.
	VkImage _VulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView _VulkanImageView;

	//The format of this depth buffer.
	VkFormat _Format;

	/*
	*	Finds and returns the most desirable format for the depth buffer.
	*/
	VkFormat FindMostDesirableDepthBufferFormat() const NOEXCEPT;

};
#endif