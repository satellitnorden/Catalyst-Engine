#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/API Layer/Vulkan/VulkanRenderPass.h>

//Forward declarations.
class VulkanDescriptorSetLayout;
class VulkanPipelineCreationParameters;
class VulkanShaderModule;

class VulkanPipeline final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanPipeline() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanPipeline() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan pipeline.
	*/
	const VkPipeline Get() const NOEXCEPT { return vulkanPipeline; }

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
	const VkPipelineLayout& GetPipelineLayout() const NOEXCEPT { return vulkanPipelineLayout; }

	/*
	*	Returns the Vulkan render pass.
	*/
	const VulkanRenderPass& GetRenderPass() const NOEXCEPT { return vulkanRenderPass; }

private:

	//The underlying Vulkan pipeline.
	VkPipeline vulkanPipeline;

	//The underlying Vulkan pipeline layout.
	VkPipelineLayout vulkanPipelineLayout;

	//The Vulkan render pass.
	VulkanRenderPass vulkanRenderPass;

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
	void CreatePipelineColorBlendAttachmentStates(DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a pipeline color state create info.
	*/
	void CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates) const NOEXCEPT;
	/*
	*	Creates a pipeline layout create info.
	*/
	void CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a graphics pipeline create info.
	*/
	void CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo *pipelineTessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const VulkanRenderPass &vulkanRenderPass) const NOEXCEPT;

};