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

	//The framebuffer associated with the graphics pipeline.
	DynamicArray<VkFramebuffer> _Framebuffers;

	//The pipeline associated with the graphics pipeline.
	VulkanGraphicsPipeline *RESTRICT _Pipeline;

	//The render pass associated with the graphics pipeline.
	VkRenderPass _RenderPass;

};
#endif