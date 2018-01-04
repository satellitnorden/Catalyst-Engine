//Header file.
#include <VulkanPipeline.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanPipeline::VulkanPipeline() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanPipeline::~VulkanPipeline() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan pipeline.
*/
void VulkanPipeline::Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT
{
	//Create the pipeline shader stage create infos for both shaders.
	DynamicArray<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	pipelineShaderStageCreateInfos.Resize(vulkanPipelineCreationParameters.shaderModules.Size());

	for (size_t i = 0, size = vulkanPipelineCreationParameters.shaderModules.Size(); i < size; ++i)
	{
		CreatePipelineShaderStageCreateInfo(pipelineShaderStageCreateInfos[i], vulkanPipelineCreationParameters.shaderModules[i]);
	}

	//Create the pipeline vertex input state create info.
	VkVertexInputBindingDescription vertexInputBindingDescription = VulkanUtilities::GetVertexInputBindingDescription();
	StaticArray<VkVertexInputAttributeDescription, 4> vertexInputAttributeDescriptions = VulkanUtilities::GetVertexInputAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
	CreatePipelineVertexInputStateCreateInfo(pipelineVertexInputStateCreateInfo, vertexInputBindingDescription, vertexInputAttributeDescriptions);

	//Create the pipeline input assembly state create info.
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
	CreatePipelineInputAssemblyStateCreateInfo(pipelineInputAssemblyStateCreateInfo);

	//Create the pipeline viewport state create info.
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
	VkViewport viewport;
	VkRect2D scissors;
	CreatePipelineViewportStateCreateInfo(pipelineViewportStateCreateInfo, viewport, scissors, vulkanPipelineCreationParameters);

	//Create the pipeline rasterization state create info.
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
	CreatePipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);

	//Create the pipeline multisample state create info.
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
	CreatePipelineMultisampleStateCreateInfo(pipelineMultisampleStateCreateInfo);

	//Create the pipeline depth stencil state create info.
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
	CreatePipelineDepthStencilStateCreateInfo(pipelineDepthStencilStateCreateInfo);

	//Create the pipeline color blend attachment state.
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
	CreatePipelineColorBlendAttachmentState(pipelineColorBlendAttachmentState);

	//Create the pipeline color blend state create info.
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
	CreatePipelineColorBlendStateCreateInfo(pipelineColorBlendStateCreateInfo, pipelineColorBlendAttachmentState);

	//Create the pipeline layout create info.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;

	DynamicArray<VkDescriptorSetLayoutBinding> descriptorSetLayoutsBindings;
	descriptorSetLayoutsBindings.Reserve(vulkanPipelineCreationParameters.descriptorLayoutBindingInformations.Size());

	for (auto &descriptorLayoutBindignInformation : vulkanPipelineCreationParameters.descriptorLayoutBindingInformations)
	{
		descriptorSetLayoutsBindings.EmplaceUnsafe(VulkanUtilities::CreateDescriptorSetLayoutBinding(descriptorLayoutBindignInformation.binding, descriptorLayoutBindignInformation.descriptorType, descriptorLayoutBindignInformation.shaderStages));
	}

	vulkanDescriptorSetLayout.Initialize(descriptorSetLayoutsBindings);

	CreatePipelineLayoutCreateInfo(pipelineLayoutCreateInfo, vulkanDescriptorSetLayout);

	//Create the Vulkan pipeline layout!
	VkResult result = vkCreatePipelineLayout(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &pipelineLayoutCreateInfo, nullptr, &vulkanPipelineLayout);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif

	//Initialize the Vulkan render pass.
	vulkanRenderPass.Initialize(vulkanPipelineCreationParameters);

	//Create the graphics pipeline create info.
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	CreateGraphicsPipelineCreateInfo(graphicsPipelineCreateInfo, pipelineShaderStageCreateInfos, pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo, pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo, pipelineColorBlendStateCreateInfo, vulkanPipelineLayout, vulkanRenderPass);

	//Create the Vulkan pipeline!
	result = vkCreateGraphicsPipelines(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &vulkanPipeline);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Releases this Vulkan pipeline.
*/
void VulkanPipeline::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan pipeline.
	vkDestroyPipeline(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanPipeline, nullptr);

	//Release the Vulkan render pass.
	vulkanRenderPass.Release();

	//Release the descriptor set layout.
	vulkanDescriptorSetLayout.Release();

	//Destroy the Vulkan pipeline layout.
	vkDestroyPipelineLayout(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanPipelineLayout, nullptr);
}

/*
*	Creates a vertex pipeline shader stage create info.
*/
void VulkanPipeline::CreatePipelineShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo, const VulkanShaderModule *CATALYST_RESTRICT vulkanShaderModule) const CATALYST_NOEXCEPT
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
void VulkanPipeline::CreatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkVertexInputBindingDescription &vertexInputBindingDescription, const StaticArray<VkVertexInputAttributeDescription, 4> &vertexInputAttributeDescriptions) const CATALYST_NOEXCEPT
{
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.pNext = nullptr;
	pipelineVertexInputStateCreateInfo.flags = 0;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.Data();
}

/*
*	Creates a pipeline inpit assembly state create info.
*/
void VulkanPipeline::CreatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo) const CATALYST_NOEXCEPT
{
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
	pipelineInputAssemblyStateCreateInfo.flags = 0;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
}

/*
*	Creates a pipeline viewport state create info.
*/
void VulkanPipeline::CreatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, VkViewport &viewport, VkRect2D &scissors, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT
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
void VulkanPipeline::CreatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo) const CATALYST_NOEXCEPT
{
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.pNext = nullptr;
	pipelineRasterizationStateCreateInfo.flags = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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
void VulkanPipeline::CreatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo) const CATALYST_NOEXCEPT
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
void VulkanPipeline::CreatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo) const CATALYST_NOEXCEPT
{
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.pNext = nullptr;
	pipelineDepthStencilStateCreateInfo.flags = 0;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.front = {};
	pipelineDepthStencilStateCreateInfo.back = {};
	pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
}

/*
*	Creates a pipeline color blend attachment state.
*/
void VulkanPipeline::CreatePipelineColorBlendAttachmentState(VkPipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState) const CATALYST_NOEXCEPT
{
	pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

/*
*	Creates a pipeline color state create info.
*/
void VulkanPipeline::CreatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState) const CATALYST_NOEXCEPT
{
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.pNext = nullptr;
	pipelineColorBlendStateCreateInfo.flags = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;
}

/*
*	Creates a pipeline layout create info.
*/
void VulkanPipeline::CreatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo, const VulkanDescriptorSetLayout &descriptorSetLayout) const CATALYST_NOEXCEPT
{
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout.Get();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
}

/*
*	Creates a graphics pipeline create info.
*/
void VulkanPipeline::CreateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo, const DynamicArray<VkPipelineShaderStageCreateInfo> &pipelineShaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo, const VkPipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo &pipelineMultisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo &pipelineDepthStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo &pipelineColorBlendStateCreateInfo, const VkPipelineLayout &pipelineLayout, const VulkanRenderPass &vulkanRenderPass) const CATALYST_NOEXCEPT
{
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(pipelineShaderStageCreateInfos.Size());
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfos.Data();
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState = nullptr;
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