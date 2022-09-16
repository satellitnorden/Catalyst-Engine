#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanCubeMapTexture final : public VulkanImage
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VulkanCubeMapTexture() NOEXCEPT
	{
		//Set the type.
		_Type = Type::VULKAN_CUBE_MAP_TEXTURE;
	}

	/*
	*	Returns the underlying Vulkan image.
	*/
	const VkImage& Get() const NOEXCEPT { return _VulkanImage; }

	/*
	*	Initializes this texture.
	*/
	void Initialize(const DynamicArray<DynamicArray<float32>> &data, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Releases this texture.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView& GetImageView() const NOEXCEPT { return _VulkanImageView; }

};
#endif