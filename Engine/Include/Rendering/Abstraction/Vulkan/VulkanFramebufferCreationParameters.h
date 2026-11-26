#if defined(CATALYST_HAS_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanFramebufferCreationParameters final
{

public:

	//The render pass.
	VkRenderPass _RenderPass;

	//The attachments.
	DynamicArray<VkImageView> _Attachments;

	//The extent.
	VkExtent2D _Extent;

	//The number of layers.
	uint32 _NumberOfLayers;

};
#endif