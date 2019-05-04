#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>

class VulkanComputePipelineCreationParameters final
{

public:

	//The descriptor set layout count.
	uint32 _DescriptorSetLayoutCount;

	//The descriptor set layouts.
	const VulkanDescriptorSetLayout *RESTRICT _DescriptorSetLayouts;

	//The number of push constant ranges.
	uint32 _PushConstantRangeCount;

	//The push constant ranges.
	VkPushConstantRange *RESTRICT _PushConstantRanges;

	//The shader module.
	VulkanShaderModule *RESTRICT _ShaderModule;

};
#endif