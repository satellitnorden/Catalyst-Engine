#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipeline.h>

class VulkanPipelineSubStageData final
{

public:

	//The framebuffer associated with the render pass.
	DynamicArray<VkFramebuffer> _Framebuffers;

	//The pipeline associated with the render pass.
	VulkanGraphicsPipeline *RESTRICT _Pipeline;

	//The render pass associated with the render pass.
	VkRenderPass _RenderPass;

};
#endif