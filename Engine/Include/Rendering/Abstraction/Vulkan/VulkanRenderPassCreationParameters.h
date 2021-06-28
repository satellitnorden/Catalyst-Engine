#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VulkanRenderPassCreationParameters final
{

public:

	//The attachment count.
	uint32 _AttachmentCount;

	//The attachment descriptions.
	const VkAttachmentDescription *RESTRICT _AttachmentDescriptions;

	//The subpass description count.
	uint32 _SubpassDescriptionCount;

	//The subpass descriptions.
	const VkSubpassDescription *RESTRICT _SubpassDescriptions;

	//The subpass dependency count.
	uint32 _SubpassDependencyCount;

	//The subpass dependencies.
	const VkSubpassDependency *RESTRICT _SubpassDependencies;

	//Denotes whether or not multiview should be enabled.
	bool _MultiviewEnabled;

};
#endif