#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipeline.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan graphics pipeline.
*/
void VulkanGraphicsPipeline::Initialize(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT
{
	//Create the pipeline shader stage create infos for both shaders.
	DynamicArray<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	pipelineShaderStageCreateInfos.Upsize<false>(parameters._ShaderModules.Size());

	bool useTessellationStage{ false };

	for (uint64 i = 0, size = parameters._ShaderModules.Size(); i < size; ++i)
	{
		CreatePipelineShaderStageCreateInfo(pipelineShaderStageCreateInfos[i], parameters._ShaderModules[i]);

		if (parameters._ShaderModules[i]->GetStage() == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT || parameters._ShaderModules[i]->GetStage() == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
		{
			useTessellationStage = true;
		}
	}

	//Create the pipeline vertex input state create info.
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
	CreatePipelineVertexInputStateCreateInfo(pipelineVertexInputStateCreateInfo, parameters);

	//Create the pipeline input assembly state create info.
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
	CreatePipelineInputAssemblyStateCreateInfo(pipelineInputAssemblyStateCreateInfo, parameters);

	//Create the pipeline tessellation create info.
	VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo;
	CreatePipelineTessellationStateCreateInfo(pipelineTessellationStateCreateInfo);

	//Create the pipeline viewport state create info.
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
	VkViewport viewport;
	VkRect2D scissors;
	CreatePipelineViewportStateCreateInfo(pipelineViewportStateCreateInfo, viewport, scissors, parameters);

	//Create the pipeline rasterization state create info.
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
	CreatePipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo, parameters);

	//Create the pipeline multisample state create info.
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
	CreatePipelineMultisampleStateCreateInfo(pipelineMultisampleStateCreateInfo, parameters);

	//Create the pipeline depth stencil state create info.
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
	CreatePipelineDepthStencilStateCreateInfo(pipelineDepthStencilStateCreateInfo, parameters);

	//Create the pipeline color blend attachment state.
	DynamicArray<VkPipelineColorBlendAttachmentState> pipelineColorBlendAttachmentStates;
	CreatePipelineColorBlendAttachmentStates(pipelineColorBlendAttachmentStates, parameters);

	//Create the pipeline color blend state create info.
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
	CreatePipelineColorBlendStateCreateInfo(pipelineColorBlendStateCreateInfo, pipelineColorBlendAttachmentStates, parameters);

	//Create the pipeline layout create info.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	CreatePipelineLayoutCreateInfo(pipelineLayoutCreateInfo, parameters);

	//Create the Vulkan pipeline layout!
	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &pipelineLayoutCreateInfo, nullptr, &_VulkanPipelineLayout));

	//Create the graphics pipeline create info.
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	CreateGraphicsPipelineCreateInfo(graphicsPipelineCreateInfo, pipelineShaderStageCreateInfos, pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, useTessellationStage ? &pipelineTessellationStateCreateInfo : nullptr, pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo, pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo, pipelineColorBlendStateCreateInfo, _VulkanPipelineLayout, parameters._Subpass, parameters);

	//Create the Vulkan graphics pipeline!
	VULKAN_ERROR_CHECK(vkCreateGraphicsPipelines(VulkanInterface::Instance->GetLogicalDevice().Get(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &_VulkanPipeline));
}

/*
*	Releases this Vulkan graphics pipeline.
*/
void VulkanGraphicsPipeline::Release() NOEXCEPT
{
	//Destroy the Vulkan pipeline layout.
	vkDestroyPipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipelineLayout, nullptr);

	//Destroy the Vulkan pipeline.
	vkDestroyPipeline(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipeline, nullptr);
}

/*
*	Creates a vertex pipeline shader stage create info.
*/
void VulkanGraphicsPipeline::CreatePipelineShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo, const VulkanShaderModule *RESTRICT vulkanShaderModule) const NOEXCEPT
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
void VulkanGraphicsPipeline::CreatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.pNext = nullptr;
	pipelineVertexInputStateCreateInfo.flags = 0;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vulkanPipelineCreationParameters._VertexInputBindingDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vulkanPipelineCreationParameters._VertexInputBindingDescriptions;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vulkanPipelineCreationParameters._VertexInputAttributeDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vulkanPipelineCreationParameters._VertexInputAttributeDescriptions;
}

/*
*	Creates a pipeline input assembly state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
	pipelineInputAssemblyStateCreateInfo.flags = 0;
	pipelineInputAssemblyStateCreateInfo.topology = vulkanPipelineCreationParameters._Topology;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
}

/*
*	Creates a pipeline tessellation state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineTessellationStateCreateInfo(VkPipelineTessellationStateCreateInfo &pipelineTessellationStateCreateInfo) const NOEXCEPT
{
	pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	pipelineTessellationStateCreateInfo.pNext = nullptr;
	pipelineTessellationStateCreateInfo.flags = 0;
	pipelineTessellationStateCreateInfo.patchControlPoints = 3;
}

/*
*	Creates a pipeline viewport state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, VkViewport &viewport, VkRect2D &scissors, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(vulkanPipelineCreationParameters._ViewportExtent.width);
	viewport.height = static_cast<float>(vulkanPipelineCreationParameters._ViewportExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissors.offset = { 0, 0 };
	scissors.extent = vulkanPipelineCreationParameters._ViewportExtent;

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
void VulkanGraphicsPipeline::CreatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.pNext = nullptr;
	pipelineRasterizationStateCreateInfo.flags = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = vulkanPipelineCreationParameters._CullMode;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
}

/*
*	Creates a pipeline multisample state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.pNext = nullptr;
	pipelineMultisampleStateCreateInfo.flags = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = vulkanPipelineCreationParameters._SampleCount;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
	pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
}

/*
*	Creates a pipeline depth stencil state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.pNext = nullptr;
	pipelineDepthStencilStateCreateInfo.flags = 0;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = vulkanPipelineCreationParameters._DepthTestEnable;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = vulkanPipelineCreationParameters._DepthWriteEnable;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = vulkanPipelineCreationParameters._DepthCompareOp;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = vulkanPipelineCreationParameters._StencilTestEnable;
	pipelineDepthStencilStateCreateInfo.front.failOp = vulkanPipelineCreationParameters._StencilFailOperator;
	pipelineDepthStencilStateCreateInfo.front.passOp = vulkanPipelineCreationParameters._StencilPassOperator;
	pipelineDepthStencilStateCreateInfo.front.depthFailOp = vulkanPipelineCreationParameters._StencilDepthFailOperator;
	pipelineDepthStencilStateCreateInfo.front.compareOp = vulkanPipelineCreationParameters._StencilCompareOperator;
	pipelineDepthStencilStateCreateInfo.front.compareMask = vulkanPipelineCreationParameters._StencilCompareMask;
	pipelineDepthStencilStateCreateInfo.front.writeMask = vulkanPipelineCreationParameters._StencilWriteMask;
	pipelineDepthStencilStateCreateInfo.front.reference = vulkanPipelineCreationParameters._StencilReferenceMask;
	pipelineDepthStencilStateCreateInfo.back = pipelineDepthStencilStateCreateInfo.front;
	pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
}

/*
*	Creates a pipeline color blend attachment state.
*/
void VulkanGraphicsPipeline::CreatePipelineColorBlendAttachmentStates(DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanGraphicsPipelineCreationParameters &parameters) const NOEXCEPT
{
	pipelineColorBlendAttachmentStates.Reserve(parameters._ColorAttachmentCount);

	for (uint32 i = 0; i < parameters._ColorAttachmentCount; ++i)
	{
		VkPipelineColorBlendAttachmentState newPipelineColorBlendAttachmentState;

		newPipelineColorBlendAttachmentState.blendEnable = parameters._BlendEnable ? VK_TRUE : VK_FALSE;
		newPipelineColorBlendAttachmentState.srcColorBlendFactor = parameters._BlendFactorSourceColor;
		newPipelineColorBlendAttachmentState.dstColorBlendFactor = parameters._BlendFactorDestinationColor;
		newPipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		newPipelineColorBlendAttachmentState.srcAlphaBlendFactor = parameters._BlendFactorSourceAlpha;
		newPipelineColorBlendAttachmentState.dstAlphaBlendFactor = parameters._BlendFactorDestinationAlpha;
		newPipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		newPipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
		pipelineColorBlendAttachmentStates.Emplace(newPipelineColorBlendAttachmentState);
	}
}

/*
*	Creates a pipeline color state create info.
*/
void VulkanGraphicsPipeline::CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const DynamicArray<VkPipelineColorBlendAttachmentState> &pipelineColorBlendAttachmentStates, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameter) const NOEXCEPT
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
void VulkanGraphicsPipeline::CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
{
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = vulkanPipelineCreationParameters._DescriptorSetLayoutCount;
	pipelineLayoutCreateInfo.pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout *RESTRICT>(vulkanPipelineCreationParameters._DescriptorSetLayouts);
	pipelineLayoutCreateInfo.pushConstantRangeCount = vulkanPipelineCreationParameters._PushConstantRangeCount;
	pipelineLayoutCreateInfo.pPushConstantRanges = vulkanPipelineCreationParameters._PushConstantRanges;
}

/*
*	Creates a graphics pipeline create info.
*/
void VulkanGraphicsPipeline::CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo *pipelineTessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const uint32 subpass, const VulkanGraphicsPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT
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
	graphicsPipelineCreateInfo.renderPass = vulkanPipelineCreationParameters._RenderPass->Get();
	graphicsPipelineCreateInfo.subpass = subpass;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;
}
#endif