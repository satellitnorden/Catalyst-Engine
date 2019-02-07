#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/API/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class Vulkan2DTexture final : public VulkanImage
{

public:

	/*
	*	Initializes this texture with void data.
	*/
	void Initialize(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format) NOEXCEPT;

	/*
	*	Releases this texture.
	*/
	void Release() NOEXCEPT;

};
#endif