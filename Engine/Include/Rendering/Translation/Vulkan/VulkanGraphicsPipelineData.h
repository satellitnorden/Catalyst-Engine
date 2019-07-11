#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipeline.h>

class VulkanGraphicsPipelineData final
{

public:


	//The pipeline associated with the graphics pipeline.
	VulkanGraphicsPipeline *RESTRICT _Pipeline;

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