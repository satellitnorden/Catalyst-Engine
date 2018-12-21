#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanRenderPassSubStageData final
{

public:

	//The framebuffer associated with the render pass.
	DynamicArray<VkFramebuffer> _Framebuffers;

	//The pipeline associated with the render pass.
	VulkanPipeline *RESTRICT _Pipeline;

	//The render pass associated with the render pass.
	VkRenderPass _RenderPass;

};
#endif