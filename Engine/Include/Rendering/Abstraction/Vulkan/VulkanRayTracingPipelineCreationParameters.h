#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>

class VulkanRayTracingPipelineCreationParameters final
{

public:

	//The shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> _ShaderModules;

};
#endif