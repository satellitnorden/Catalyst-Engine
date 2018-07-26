#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanRenderPassData final
{

public:

	//The framebuffer associated with the render pass.
	DynamicArray<VkFramebuffer> framebuffers;

	//The pipeline associated with the render pass.
	VkPipeline pipeline;

	//The pipeline layout associated with the render pass.
	VkPipelineLayout pipelineLayout;

	//The render pass associated with the render pass.
	VkRenderPass renderPass;

};