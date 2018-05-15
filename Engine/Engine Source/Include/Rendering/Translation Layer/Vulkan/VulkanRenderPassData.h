#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanRenderPassData final
{

public:

	//The framebuffer associated with the render pass.
	VkFramebuffer framebuffer;

	//The pipeline associated with the render pass.
	VkPipeline pipeline;

	//The pipeline layout associated with the render pass.
	VkPipelineLayout pipelineLayout;

	//The render pass associated with the render pass.
	VkRenderPass renderPass;

};