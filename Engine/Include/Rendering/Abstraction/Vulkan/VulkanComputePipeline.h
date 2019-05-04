#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanComputePipelineCreationParameters.h>

class VulkanComputePipeline final
{

public:

	/*
	*	Initializes this Vulkan compute pipeline.
	*/
	void Initialize(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Releases this Vulkan compute pipeline.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan pipeline.
	*/
	const VkPipeline& GetPipeline() const NOEXCEPT
	{
		return _VulkanPipeline;
	}

	/*
	*	Returns the underlying Vulkan pipeline layout.
	*/
	const VkPipelineLayout& GetPipelineLayout() const NOEXCEPT
	{
		return _VulkanPipelineLayout;
	}

private:

	//The underlying Vulkan pipeline.
	VkPipeline _VulkanPipeline;

	//The underlying Vulkan pipeline layout.
	VkPipelineLayout _VulkanPipelineLayout;

	/*
	*	Creates a pipeline layout create info.
	*/
	void CreatePipelineLayoutCreateInfo(const VulkanComputePipelineCreationParameters &parameters, VkPipelineLayoutCreateInfo *const RESTRICT pipelineLayoutCreateInfo) const NOEXCEPT;

	/*
	*	Creates the compute pipeline create info.
	*/
	void CreateComputegPipelineCreateInfo(const VulkanComputePipelineCreationParameters &parameters, VkComputePipelineCreateInfo *const RESTRICT computePipelineCreateInfo) const NOEXCEPT;

};
#endif