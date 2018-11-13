#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanImage
{

public:

	/*
	*	Returns the Vulkan image.
	*/
	const VkImage GetImage() const NOEXCEPT { return _VulkanImage; }

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView GetImageView() const NOEXCEPT { return _VulkanImageView; }

protected:

	//The Vulkan image.
	VkImage _VulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView _VulkanImageView;


};