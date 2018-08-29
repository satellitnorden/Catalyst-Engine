#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanDepthBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDepthBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDepthBuffer() NOEXCEPT;

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
	const VkImageView& GetImageView() const NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the format of this Vulkan depth buffer.
	*/
	const VkFormat& GetFormat() const NOEXCEPT { return format; }

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
	VkFormat FindMostDesirableDepthBufferFormat() const NOEXCEPT;

};