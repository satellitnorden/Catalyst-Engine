#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSetLayout.h>

//Forward declarations.
class VulkanDescriptorSetLayout;
class VulkanPipelineCreationParameters;
class VulkanShaderModule;

class VulkanPipeline final
{

public:

	/*
	*	Returns the underlying Vulkan pipeline.
	*/
	const VkPipeline Get() const NOEXCEPT { return _VulkanPipeline; }

	/*
	*	Initializes this Vulkan pipeline.
	*/
	void Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT;

	/*
	*	Releases this Vulkan pipeline.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the Vulkan pipeline layout.
	*/
	const VkPipelineLayout& GetPipelineLayout() const NOEXCEPT { return _VulkanPipelineLayout; }

private:

	//The underlying Vulkan pipeline.
	VkPipeline _VulkanPipeline;

	//The underlying Vulkan pipeline layout.
	VkPipelineLayout _VulkanPipelineLayout;

	/*
	*	Creates a vertex pipeline shader stage create info.
	*/
	void CreatePipelineShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo, const VulkanShaderModule *RESTRICT vulkanShaderModule) const NOEXCEPT;

	/*
	*	Creates a pipeline vertex input state create info.
	*/
	void CreatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline input assembly state create info.
	*/
	void CreatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline tessellation state create info.
	*/
	void CreatePipelineTessellationStateCreateInfo(VkPipelineTessellationStateCreateInfo &pipelineTessellationStateCreateInfo) const NOEXCEPT;

	/*
	*	Creates a pipeline viewport state create info.
	*/
	void CreatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, VkViewport &viewport, VkRect2D &scissors, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline rasterization state create info.
	*/
	void CreatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline multisample state create info.
	*/
	void CreatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo) const NOEXCEPT;

	/*
	*	Creates a pipeline depth stencil state create info.
	*/
	void CreatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline color blend attachment state.
	*/
	void CreatePipelineColorBlendAttachmentStates(DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanPipelineCreationParameters &parameters) const NOEXCEPT;

	/*
	*	Creates a pipeline color state create info.
	*/
	void CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameter) const NOEXCEPT;
	
	/*
	*	Creates a pipeline layout create info.
	*/
	void CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a graphics pipeline create info.
	*/
	void CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo *pipelineTessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const uint32 subpass, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

};
#endif