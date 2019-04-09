#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>

class VulkanRayTracingPipelineCreationParameters final
{

public:

	//The descriptor set layout count.
	uint32 _DescriptorSetLayoutCount;

	//The descriptor set layouts.
	const VulkanDescriptorSetLayout *RESTRICT _DescriptorSetLayouts;

	//The shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> _ShaderModules;

};
#endif