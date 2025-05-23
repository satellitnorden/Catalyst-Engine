#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanMemoryAllocator.h>

class VulkanImage
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		VULKAN_2D_TEXTURE,
		VULKAN_3D_TEXTURE,
		VULKAN_CUBE_MAP_TEXTURE,
		VULKAN_RENDER_TARGET,
		VULKAN_DEPTH_BUFFER
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

	/*
	*	Returns the extent.
	*/
	const VkExtent3D GetExtent() const NOEXCEPT
	{
		return _Extent;
	}

protected:

	//The type.
	Type _Type;

	//The Vulkan image.
	VkImage _VulkanImage{ VK_NULL_HANDLE };

	//The allocation.
	VmaAllocation _Allocation;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView _VulkanImageView{ VK_NULL_HANDLE };

	//The underlying Vulkan format.
	VkFormat _VulkanFormat;

	//The underlying Vulkan image layout.
	VkImageLayout _VulkanImageLayout;

	//The extent.
	VkExtent3D _Extent;

};
#endif