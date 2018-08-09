#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanRenderPassCreationParameters final
{

public:

	//The number of attachments.
	uint32 attachmentCount;

	//The attachment descriptions.
	const VkAttachmentDescription* RESTRICT attachmentDescriptions;

};