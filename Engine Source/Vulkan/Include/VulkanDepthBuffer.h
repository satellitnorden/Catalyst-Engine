#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanDepthBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDepthBuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDepthBuffer() CATALYST_NOEXCEPT;

	/*
	*	Initializes this Vulkan depth buffer.
	*/
	void Initialize(const VkExtent2D imageExtent) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan depth buffer.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the image view for this Vulkan depth buffer.
	*/
	const VkImageView& GetImageView() const CATALYST_NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the format of this Vulkan depth buffer.
	*/
	const VkFormat& GetFormat() const CATALYST_NOEXCEPT { return format; }

private:

	//The Vulkan image.
	VkImage vulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView vulkanImageView;

	//The format of this depth buffer.
	VkFormat format;

	/*
	*	Finds and returns the most desirable format for the depth buffer.
	*/
	VkFormat FindMostDesirableDepthBufferFormat() const CATALYST_NOEXCEPT;

};