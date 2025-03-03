#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class Vulkan2DTexture final : public VulkanImage
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Vulkan2DTexture() NOEXCEPT
	{
		//Set the type.
		_Type = Type::VULKAN_2D_TEXTURE;
	}

	/*
	*	Initializes this texture with void data.
	*/
	void Initialize(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkImageUsageFlags image_usage_flags, const bool updatable) NOEXCEPT;

	/*
	*	Updates this texture.
	*/
	void Update(const void *RESTRICT const *RESTRICT texture_data) NOEXCEPT; 

	/*
	*	Releases this texture.
	*/
	void Release() NOEXCEPT;

private:

	//The texture width.
	uint32 _TextureWidth;

	//The texture height.
	uint32 _TextureHeight;

	//The texture mipmap levels.
	uint32 _TextureMipmapLevels;

	//The texture channels.
	uint32 _TextureChannels;

	//The texture texel size.
	uint32 _TextureTexelSize;

};
#endif