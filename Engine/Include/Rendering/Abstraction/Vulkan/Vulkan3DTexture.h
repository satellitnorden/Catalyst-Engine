#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class Vulkan3DTexture final : public VulkanImage
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Vulkan3DTexture() NOEXCEPT
	{
		//Set the type.
		_Type = Type::VULKAN_3D_TEXTURE;
	}

	/*
	*	Initializes this texture with void data.
	*/
	void Initialize(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureDepth, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format) NOEXCEPT;

	/*
	*	Releases this texture.
	*/
	void Release() NOEXCEPT;

};
#endif