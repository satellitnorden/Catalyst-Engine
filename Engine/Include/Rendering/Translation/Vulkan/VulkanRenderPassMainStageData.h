#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanRenderPassMainStageData final
{

public:

	//The render pass.
	VulkanRenderPass *RESTRICT _RenderPass;

	//The framebuffers.
	DynamicArray<VulkanFramebuffer *RESTRICT> _FrameBuffers;

	//Defines whether or not to clear the framebuffers.
	bool _ShouldClear;

	//The number of attachments.
	uint32 _NumberOfAttachments;

};
#endif