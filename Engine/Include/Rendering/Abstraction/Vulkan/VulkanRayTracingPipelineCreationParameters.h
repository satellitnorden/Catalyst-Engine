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

	/*
	*	Vulkan hit group class definition.
	*/
	class VulkanHitGroup final
	{

	public:

		//The closest hit shader.
		const VulkanShaderModule *RESTRICT _ClosestHitShader;

		//The any hit shader.
		const VulkanShaderModule *RESTRICT _AnyHitShader;

		//The intersection shader.
		const VulkanShaderModule *RESTRICT _IntersectionShader;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE constexpr VulkanHitGroup(	const VulkanShaderModule *const RESTRICT initial_closest_hit_shader,
												const VulkanShaderModule *const RESTRICT initial_any_hit_shader,
												const VulkanShaderModule *const RESTRICT initial_intersection_shader) NOEXCEPT
			:
			_ClosestHitShader(initial_closest_hit_shader),
			_AnyHitShader(initial_any_hit_shader),
			_IntersectionShader(initial_intersection_shader)
		{

		}

	};

	//The descriptor set layout count.
	uint32 _DescriptorSetLayoutCount;

	//The descriptor set layouts.
	const VulkanDescriptorSetLayout *RESTRICT _DescriptorSetLayouts;

	//The number of push constant ranges.
	uint32 _PushConstantRangeCount;

	//The push constant ranges.
	VkPushConstantRange *RESTRICT _PushConstantRanges;

	//The ray generation shader module.
	VulkanShaderModule *RESTRICT _RayGenerationShaderModule;

	//The hit groups
	DynamicArray<VulkanHitGroup> _HitGroups;

	//The miss shader modules.
	DynamicArray<const VulkanShaderModule *RESTRICT> _MissShaderModules;

};
#endif