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

	/*
	*	Returns the Vulkan format.
	*/
	const VkFormat GetFormat() const NOEXCEPT { return _VulkanFormat; }

	/*
	*	Returns the Vulkan image layout.
	*/
	const VkImageLayout GetImageLayout() const NOEXCEPT { return _VulkanImageLayout; }

protected:

	//The Vulkan image.
	VkImage _VulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView _VulkanImageView;

	//The underlying Vulkan format.
	VkFormat _VulkanFormat;

	//The underlying Vulkan image layout.
	VkImageLayout _VulkanImageLayout;

};