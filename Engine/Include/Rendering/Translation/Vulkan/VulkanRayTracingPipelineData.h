#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanRayTracingPipeline.h>

class VulkanRayTracingPipelineData final
{

public:

	//The pipeline associated with this ray tracing pipeline.
	VulkanRayTracingPipeline *RESTRICT _Pipeline;

	//The shader binding table buffer.
	VulkanBuffer *RESTRICT _ShaderBindingTableBuffer;

};
#endif