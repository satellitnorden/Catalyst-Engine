#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanImage.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanCubeMapTexture final : public VulkanImage
{

public:

	/*
	*	Returns the underlying Vulkan image.
	*/
	const VkImage& Get() const NOEXCEPT { return _VulkanImage; }

	/*
	*	Initializes this texture.
	*/
	void Initialize(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT;

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