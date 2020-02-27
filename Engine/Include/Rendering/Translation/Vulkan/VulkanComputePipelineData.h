#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanComputePipeline.h>
#include <Rendering/Translation/Vulkan/VulkanPipelineData.h>

class VulkanComputePipelineData final : public VulkanPipelineData
{

public:

	//The pipeline associated with this compute pipeline.
	VulkanComputePipeline *RESTRICT _Pipeline;

};
#endif