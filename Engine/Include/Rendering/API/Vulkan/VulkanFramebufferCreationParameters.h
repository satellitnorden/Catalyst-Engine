#pragma once

//Core.
#include <Core/General/EngineCore.h>

class VulkanFramebufferCreationParameters final
{

public:

	//The render pass.
	VkRenderPass renderPass;

	//The attachment count.
	uint32 attachmentCount;

	//The attachments.
	const VkImageView *RESTRICT attachments;

	//The extent.
	VkExtent2D extent;

};