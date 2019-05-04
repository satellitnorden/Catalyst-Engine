#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanComputePipeline.h>

class VulkanComputePipelineData final
{

public:

	//The pipeline associated with this compute pipeline.
	VulkanComputePipeline *RESTRICT _Pipeline;

};
#endif