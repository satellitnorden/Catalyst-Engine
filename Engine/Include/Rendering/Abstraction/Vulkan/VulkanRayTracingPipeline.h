#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanRayTracingPipelineCreationParameters.h>

class VulkanRayTracingPipeline final
{

public:

	/*
	*	Initializes this Vulkan ray tracing pipeline.
	*/
	void Initialize(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Releases this Vulkan ray tracing pipeline.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan pipeline.
	VkPipeline _VulkanPipeline;

	//The underlying Vulkan pipeline layout.
	VkPipelineLayout _VulkanPipelineLayout;

	/*
	*	Creates the stages.
	*/
	void CreateStages(const VulkanRayTracingPipelineCreationParameters &parameters, DynamicArray<VkPipelineShaderStageCreateInfo> *const RESTRICT stages) const NOEXCEPT;

	/*
	*	Creates the groups.
	*/
	void CreateGroups(const VulkanRayTracingPipelineCreationParameters &parameters, DynamicArray<VkRayTracingShaderGroupCreateInfoNV> *const RESTRICT groups) const NOEXCEPT;

	/*
	*	Creates a pipeline layout create info.
	*/
	void CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo *const RESTRICT pipelineLayoutCreateInfo) const NOEXCEPT;

	/*
	*	Creates the ray tracing pipeline create info.
	*/
	void CreateRayTracingPipelineCreateInfo(const ArrayProxy<VkPipelineShaderStageCreateInfo> &stages,
											const ArrayProxy<VkRayTracingShaderGroupCreateInfoNV> &groups,
											const VkPipelineLayout &layout,
											VkRayTracingPipelineCreateInfoNV *const RESTRICT rayTracingPipelineCreateInfo) const NOEXCEPT;

};