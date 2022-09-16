#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanImage
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		VULKAN_2D_TEXTURE,
		VULKAN_3D_TEXTURE,
		VULKAN_CUBE_MAP_TEXTURE,
		VULKAN_RENDER_TARGET
	};

	/*
	*	Returns the type.
	*/
	FORCE_INLINE NO_DISCARD Type GetType() const NOEXCEPT
	{
		return _Type;
	}

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

	//The type.
	Type _Type;

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
#endif