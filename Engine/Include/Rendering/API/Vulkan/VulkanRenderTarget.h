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

class VulkanRenderTarget final : public VulkanImage
{

public:

	/*
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent, const VkFormat format) NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the extent of the render target.
	*/
	VkExtent2D GetExtent() const NOEXCEPT { return _Extent; }

private:

	//The extent of the image.
	VkExtent2D _Extent;

};
#endif