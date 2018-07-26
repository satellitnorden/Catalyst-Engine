#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanRenderPassCreationParameters final
{

public:

	//The number of attachments.
	uint32 attachmentCount;

	//The attachment descriptions.
	const VkAttachmentDescription* RESTRICT attachmentDescriptions;

};