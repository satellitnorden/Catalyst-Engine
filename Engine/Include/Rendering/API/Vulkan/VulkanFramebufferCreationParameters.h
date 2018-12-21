#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanFramebufferCreationParameters final
{

public:

	//The render pass.
	VkRenderPass _RenderPass;

	//The attachment count.
	uint32 _AttachmentCount;

	//The attachments.
	const VkImageView *RESTRICT _Attachments;

	//The extent.
	VkExtent2D _Extent;

};
#endif