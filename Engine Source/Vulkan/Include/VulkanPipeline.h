#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Vulkan.
#include <VulkanDescriptorSet.h>
#include <VulkanDescriptorSetLayout.h>
#include <VulkanRenderPass.h>

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
	VulkanPipeline() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanPipeline() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan pipeline.
	*/
	const VkPipeline Get() const CATALYST_NOEXCEPT { return vulkanPipeline; }

	/*
	*	Initializes this Vulkan pipeline.
	*/
	void Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan pipeline.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the Vulkan pipeline layout.
	*/
	const VkPipelineLayout& GetPipelineLayout() const CATALYST_NOEXCEPT { return vulkanPipelineLayout; }

	/*
	*	Returns the Vulkan render pass.
	*/
	const VulkanRenderPass& GetRenderPass() const CATALYST_NOEXCEPT { return vulkanRenderPass; }

	/*
	*	Returns the Vulkan Descriptor set.
	*/
	const VulkanDescriptorSetLayout& GetDescriptorSetLayout() const CATALYST_NOEXCEPT { return vulkanDescriptorSetLayout; }

private:

	//The underlying Vulkan pipeline.
	VkPipeline vulkanPipeline;

	//The underlying Vulkan pipeline layout.
	VkPipelineLayout vulkanPipelineLayout;

	//The Vulkan render pass.
	VulkanRenderPass vulkanRenderPass;

	//The Vulkan descriptor set layout.
	VulkanDescriptorSetLayout vulkanDescriptorSetLayout;

	/*
	*	Creates a vertex pipeline shader stage create info.
	*/
	void CreatePipelineShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo, const VulkanShaderModule *CATALYST_RESTRICT vulkanShaderModule) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline vertex input state create info.
	*/
	void CreatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkVertexInputBindingDescription &vertexInputBindingDescription, const StaticArray<VkVertexInputAttributeDescription, 4> &vertexInputAttributeDescriptions) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline inpit assembly state create info.
	*/
	void CreatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline viewport state create info.
	*/
	void CreatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, VkViewport &viewport, VkRect2D &scissors, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline rasterization state create info.
	*/
	void CreatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline multisample state create info.
	*/
	void CreatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline depth stencil state create info.
	*/
	void CreatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline color blend attachment state.
	*/
	void CreatePipelineColorBlendAttachmentState(VkPipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState) const CATALYST_NOEXCEPT;

	/*
	*	Creates a pipeline color state create info.
	*/
	void CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState) const CATALYST_NOEXCEPT;
	/*
	*	Creates a pipeline layout create info.
	*/
	void CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanDescriptorSetLayout &descriptorSetLayout) const CATALYST_NOEXCEPT;

	/*
	*	Creates a graphics pipeline create info.
	*/
	void CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const VulkanRenderPass &vulkanRenderPass) const CATALYST_NOEXCEPT;

};