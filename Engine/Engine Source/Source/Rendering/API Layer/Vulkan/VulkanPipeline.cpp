//Header file.
#include <Rendering/API Layer/Vulkan/VulkanPipeline.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanPipeline::VulkanPipeline() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanPipeline::~VulkanPipeline() NOEXCEPT
{

}

/*
*	Initializes this Vulkan pipeline.
*/
void VulkanPipeline::Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT
{
	//Create the pipeline shader stage create infos for both shaders.
	DynamicArray<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	pipelineShaderStageCreateInfos.UpsizeFast(vulkanPipelineCreationParameters.shaderModules.Size());

	bool useTessellationStage{ false };

	for (uint64 i = 0, size = vulkanPipelineCreationParameters.shaderModules.Size(); i < size; ++i)
	{
		CreatePipelineShaderStageCreateInfo(pipelineShaderStageCreateInfos[i], vulkanPipelineCreationParameters.shaderModules[i]);

		if (vulkanPipelineCreationParameters.shaderModules[i]->GetStage() == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT || vulkanPipelineCreationParameters.shaderModules[i]->GetStage() == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
		{
			useTessellationStage = true;
		}
	}

	//Create the pipeline vertex input state create info.
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
	CreatePipelineVertexInputStateCreateInfo(pipelineVertexInputStateCreateInfo, vulkanPipelineCreationParameters);

	//Create the pipeline input assembly state create info.
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
	CreatePipelineInputAssemblyStateCreateInfo(pipelineInputAssemblyStateCreateInfo, vulkanPipelineCreationParameters);

	//Create the pipeline tessellation create info.
	VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo;
	CreatePipelineTessellationStateCreateInfo(pipelineTessellationStateCreateInfo);

	//Create the pipeline viewport state create info.
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
	VkViewport viewport;
	VkRect2D scissors;
	CreatePipelineViewportStateCreateInfo(pipelineViewportStateCreateInfo, viewport, scissors, vulkanPipelineCreationParameters);

	//Create the pipeline rasterization state create info.
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
	CreatePipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo, vulkanPipelineCreationParameters);

	//Create the pipeline multisample state create info.
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
	CreatePipelineMultisampleStateCreateInfo(pipelineMultisampleStateCreateInfo);

	//Create the pipeline depth stencil state create info.
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
	CreatePipelineDepthStencilStateCreateInfo(pipelineDepthStencilStateCreateInfo, vulkanPipelineCreationParameters);

	//Create the pipeline color blend attachment state.
	DynamicArray<VkPipelineColorBlendAttachmentState> pipelineColorBlendAttachmentStates;
	CreatePipelineColorBlendAttachmentStates(pipelineColorBlendAttachmentStates, vulkanPipelineCreationParameters);

	//Create the pipeline color blend state create info.
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
	CreatePipelineColorBlendStateCreateInfo(pipelineColorBlendStateCreateInfo, pipelineColorBlendAttachmentStates);

	//Create the pipeline layout create info.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	CreatePipelineLayoutCreateInfo(pipelineLayoutCreateInfo, vulkanPipelineCreationParameters);

	//Create the Vulkan pipeline layout!
	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &pipelineLayoutCreateInfo, nullptr, &vulkanPipelineLayout));

	//Initialize the Vulkan render pass.
	vulkanRenderPass.Initialize(vulkanPipelineCreationParameters);

	//Create the graphics pipeline create info.
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	CreateGraphicsPipelineCreateInfo(graphicsPipelineCreateInfo, pipelineShaderStageCreateInfos, pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, useTessellationStage ? &pipelineTessellationStateCreateInfo : nullptr, pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo, pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo, pipelineColorBlendStateCreateInfo, vulkanPipelineLayout, vulkanRenderPass);

	//Create the Vulkan pipeline!
	VULKAN_ERROR_CHECK(vkCreateGraphicsPipelines(VulkanInterface::Instance->GetLogicalDevice().Get(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &vulkanPipeline));
}

/*
*	Releases this Vulkan pipeline.
*/
void VulkanPipeline::Release() NOEXCEPT
{
	//Destroy the Vulkan pipeline.
	vkDestroyPipeline(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanPipeline, nullptr);

	//Release the Vulkan render pass.
	vulkanRenderPass.Release();

	//Destroy the Vulkan pipeline layout.
	vkDestroyPipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanPipelineLayout, nullptr);
}

/*
*	Creates a vertex pipeline shader stage create info.
*/
void VulkanPipeline::CreatePipelineShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo, const VulkanShaderModule *RESTRICT vulkanShaderModule) const NOEXCEPT
{
	pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo.pNext = nullptr;
	pipelineShaderStageCreateInfo.flags = 0;
	pipelineShaderStageCreateInfo.stage = vulkanShaderModule->GetStage();
	pipelineShaderStageCreateInfo.module = vulkanShaderModule->Get();
	pipelineShaderStageCreateInfo.pName = "main";
	pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
}

/*
*	Creates a pipeline vertex input state create info.
*/
void VulkanPipeline::CreatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.pNext = nullptr;
	pipelineVertexInputStateCreateInfo.flags = 0;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vulkanPipelineCreationParameters.vertexInputBindingDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vulkanPipelineCreationParameters.vertexInputBindingDescriptions;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vulkanPipelineCreationParameters.vertexInputAttributeDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vulkanPipelineCreationParameters.vertexInputAttributeDescriptions;
}

/*
*	Creates a pipeline input assembly state create info.
*/
void VulkanPipeline::CreatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
	pipelineInputAssemblyStateCreateInfo.flags = 0;
	pipelineInputAssemblyStateCreateInfo.topology = vulkanPipelineCreationParameters.topology;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
}

/*
*	Creates a pipeline tessellation state create info.
*/
void VulkanPipeline::CreatePipelineTessellationStateCreateInfo(VkPipelineTessellationStateCreateInfo &pipelineTessellationStateCreateInfo) const NOEXCEPT
{
	pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	pipelineTessellationStateCreateInfo.pNext = nullptr;
	pipelineTessellationStateCreateInfo.flags = 0;
	pipelineTessellationStateCreateInfo.patchControlPoints = 3;
}

/*
*	Creates a pipeline viewport state create info.
*/
void VulkanPipeline::CreatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, VkViewport &viewport, VkRect2D &scissors, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(vulkanPipelineCreationParameters.viewportExtent.width);
	viewport.height = static_cast<float>(vulkanPipelineCreationParameters.viewportExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissors.offset = { 0, 0 };
	scissors.extent = vulkanPipelineCreationParameters.viewportExtent;

	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.pNext = nullptr;
	pipelineViewportStateCreateInfo.flags = 0;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissors;
}

/*
*	Creates a pipeline rasterization state create info.
*/
void VulkanPipeline::CreatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.pNext = nullptr;
	pipelineRasterizationStateCreateInfo.flags = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = vulkanPipelineCreationParameters.cullMode;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
}

/*
*	Creates a pipeline multisample state create info.
*/
void VulkanPipeline::CreatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo) const NOEXCEPT
{
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.pNext = nullptr;
	pipelineMultisampleStateCreateInfo.flags = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
	pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
}

/*
*	Creates a pipeline depth stencil state create info.
*/
void VulkanPipeline::CreatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.pNext = nullptr;
	pipelineDepthStencilStateCreateInfo.flags = 0;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = vulkanPipelineCreationParameters.depthTestEnable;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = vulkanPipelineCreationParameters.depthWriteEnable;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = vulkanPipelineCreationParameters.depthCompareOp;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.front = { };
	pipelineDepthStencilStateCreateInfo.back = { };
	pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
}

/*
*	Creates a pipeline color blend attachment state.
*/
void VulkanPipeline::CreatePipelineColorBlendAttachmentStates(DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	for (VkImageView colorAttachment : vulkanPipelineCreationParameters.colorAttachments[0])
	{
		VkPipelineColorBlendAttachmentState newPipelineColorBlendAttachmentState;

		newPipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		newPipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		newPipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		newPipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		newPipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		newPipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		newPipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		newPipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		pipelineColorBlendAttachmentStates.EmplaceSlow(newPipelineColorBlendAttachmentState);
	}
}

/*
*	Creates a pipeline color state create info.
*/
void VulkanPipeline::CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates) const NOEXCEPT
{
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.pNext = nullptr;
	pipelineColorBlendStateCreateInfo.flags = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount = static_cast<uint32>(pipelineColorBlendAttachmentStates.Size());
	pipelineColorBlendStateCreateInfo.pAttachments = pipelineColorBlendAttachmentStates.Data();
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;
}

/*
*	Creates a pipeline layout create info.
*/
void VulkanPipeline::CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = vulkanPipelineCreationParameters.descriptorSetLayoutCount;
	pipelineLayoutCreateInfo.pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout *RESTRICT>(vulkanPipelineCreationParameters.descriptorSetLayouts);
	pipelineLayoutCreateInfo.pushConstantRangeCount = vulkanPipelineCreationParameters.pushConstantRangeCount;
	pipelineLayoutCreateInfo.pPushConstantRanges = vulkanPipelineCreationParameters.pushConstantRanges;
}

/*
*	Creates a graphics pipeline create info.
*/
void VulkanPipeline::CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo *pipelineTessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const VulkanRenderPass &vulkanRenderPass) const NOEXCEPT
{
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(pipelineShaderStageCreateInfos.Size());
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfos.Data();
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState = pipelineTessellationStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = nullptr;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = vulkanRenderPass.Get();
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;
}