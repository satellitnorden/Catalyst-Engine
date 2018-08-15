#pragma once

//Core.
#include <Core/EngineCore.h>

class VulkanRenderPassCreationParameters final
{

public:

	//The attachment count.
	uint32 attachmentCount;

	//The attachment descriptions.
	const VkAttachmentDescription *RESTRICT attachmentDescriptions;

	//The subpass description count.
	uint32 subpassDescriptionCount;

	//The subpass descriptions.
	const VkSubpassDescription *RESTRICT subpassDescriptions;

	//The subpass dependency count.
	uint32 subpassDependencyCount;

	//The subpass dependencies.
	const VkSubpassDependency *RESTRICT subpassDependencies;

};