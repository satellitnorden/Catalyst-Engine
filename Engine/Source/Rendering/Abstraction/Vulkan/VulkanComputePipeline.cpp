#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanComputePipeline.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan compute pipeline.
*/
void VulkanComputePipeline::Initialize(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT
{
	//Create the pipeline layout create info.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	CreatePipelineLayoutCreateInfo(parameters, &pipelineLayoutCreateInfo);

	//Create the Vulkan pipeline layout!
	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &pipelineLayoutCreateInfo, nullptr, &_VulkanPipelineLayout));

	//Create the pipeline compute create info.
	VkComputePipelineCreateInfo computePipelineCreateInfo;
	CreateComputegPipelineCreateInfo(parameters, &computePipelineCreateInfo);

	//Create the Vulkan compute pipeline!
	VULKAN_ERROR_CHECK(vkCreateComputePipelines(VulkanInterface::Instance->GetLogicalDevice().Get(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &_VulkanPipeline));
}

/*
*	Releases this Vulkan compute pipeline.
*/
void VulkanComputePipeline::Release() NOEXCEPT
{
	//Destroy the Vulkan pipeline layout.
	vkDestroyPipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipelineLayout, nullptr);

	//Destroy the pipeline.
	vkDestroyPipeline(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipeline, nullptr);
}

/*
*	Creates a pipeline layout create info.
*/
void VulkanComputePipeline::CreatePipelineLayoutCreateInfo(const VulkanComputePipelineCreationParameters &parameters, VkPipelineLayoutCreateInfo *const RESTRICT pipelineLayoutCreateInfo) const NOEXCEPT
{
	pipelineLayoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo->pNext = nullptr;
	pipelineLayoutCreateInfo->flags = 0;
	pipelineLayoutCreateInfo->setLayoutCount = parameters._DescriptorSetLayoutCount;
	pipelineLayoutCreateInfo->pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout *RESTRICT>(parameters._DescriptorSetLayouts);
	pipelineLayoutCreateInfo->pushConstantRangeCount = parameters._PushConstantRangeCount;
	pipelineLayoutCreateInfo->pPushConstantRanges = parameters._PushConstantRanges;
}

/*
*	Creates the compute pipeline create info.
*/
void VulkanComputePipeline::CreateComputegPipelineCreateInfo(const VulkanComputePipelineCreationParameters &parameters, VkComputePipelineCreateInfo *const RESTRICT computePipelineCreateInfo) const NOEXCEPT
{
	computePipelineCreateInfo->sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo->pNext = nullptr;
	computePipelineCreateInfo->flags = 0;
	computePipelineCreateInfo->stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computePipelineCreateInfo->stage.pNext = nullptr;
	computePipelineCreateInfo->stage.flags = 0;
	computePipelineCreateInfo->stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computePipelineCreateInfo->stage.module = parameters._ShaderModule->Get();
	computePipelineCreateInfo->stage.pName = "main";
	computePipelineCreateInfo->stage.pSpecializationInfo = nullptr;
	computePipelineCreateInfo->layout = _VulkanPipelineLayout;
	computePipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	computePipelineCreateInfo->basePipelineIndex = -1;
}
#endif