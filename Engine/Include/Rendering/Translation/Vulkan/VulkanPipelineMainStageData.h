#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>

class VulkanPipelineMainStageData final
{

public:

	//The render pass.
	VulkanRenderPass *RESTRICT _RenderPass;

	//The framebuffers.
	DynamicArray<VulkanFramebuffer *RESTRICT> _FrameBuffers;

	//The extent.
	VkExtent2D _Extent;

	//The number of attachments.
	uint32 _NumberOfAttachments;

	//Denotes whether or not this render pass is aimed to be rendered unto the screen.
	bool _RenderToScreeen;

};
#endif