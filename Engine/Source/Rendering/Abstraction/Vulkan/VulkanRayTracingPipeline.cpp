//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanRayTracingPipeline.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan ray tracing pipeline.
*/
void VulkanRayTracingPipeline::Initialize(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT
{
	//Create the stages.
	DynamicArray<VkPipelineShaderStageCreateInfo> stages;
	CreateStages(parameters, &stages);

	//Create the groups.
	DynamicArray<VkRayTracingShaderGroupCreateInfoNV> groups;
	CreateGroups(parameters, &groups);

	//Create the pipeline layout create info.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	CreatePipelineLayoutCreateInfo(parameters, &pipelineLayoutCreateInfo);

	//Create the Vulkan pipeline layout!
	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &pipelineLayoutCreateInfo, nullptr, &_VulkanPipelineLayout));

	//Create the ray tracing pipeline create info.
	VkRayTracingPipelineCreateInfoNV rayTracingPipelineCreateInfo;
	CreateRayTracingPipelineCreateInfo(ArrayProxy<VkPipelineShaderStageCreateInfo>(stages), ArrayProxy<VkRayTracingShaderGroupCreateInfoNV>(groups), _VulkanPipelineLayout, &rayTracingPipelineCreateInfo);

	//Create the Vulkan ray tracing pipeline!
	VULKAN_ERROR_CHECK(vkCreateRayTracingPipelinesNV(VulkanInterface::Instance->GetLogicalDevice().Get(), VK_NULL_HANDLE, 1, &rayTracingPipelineCreateInfo, nullptr, &_VulkanPipeline));
}

/*
*	Releases this Vulkan ray tracing pipeline.
*/
void VulkanRayTracingPipeline::Release() NOEXCEPT
{
	//Destroy the Vulkan pipeline layout.
	vkDestroyPipelineLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipelineLayout, nullptr);

	//Destroy the pipeline.
	vkDestroyPipeline(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipeline, nullptr);
}

/*
*	Creates the stages.
*/
void VulkanRayTracingPipeline::CreateStages(const VulkanRayTracingPipelineCreationParameters &parameters, DynamicArray<VkPipelineShaderStageCreateInfo> *const RESTRICT stages) const NOEXCEPT
{
	//Reserve the appropriate size.
	stages->Reserve(parameters._ShaderModules.Size());

	for (const VulkanShaderModule *const RESTRICT shaderModule : parameters._ShaderModules)
	{
		//Create the pipeline shader stage create info.
		stages->EmplaceFast();

		VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = 0;
		pipelineShaderStageCreateInfo.stage = shaderModule->GetStage();
		pipelineShaderStageCreateInfo.module = shaderModule->Get();
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
	}
}

/*
*	Creates the groups.
*/
void VulkanRayTracingPipeline::CreateGroups(const VulkanRayTracingPipelineCreationParameters &parameters, DynamicArray<VkRayTracingShaderGroupCreateInfoNV> *const RESTRICT groups) const NOEXCEPT
{
	groups->Reserve(3);

	groups->EmplaceFast();

	groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
	groups->Back().pNext = nullptr;
	groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
	groups->Back().generalShader = 0;
	groups->Back().closestHitShader = VK_SHADER_UNUSED_NV;
	groups->Back().anyHitShader = VK_SHADER_UNUSED_NV;
	groups->Back().intersectionShader = VK_SHADER_UNUSED_NV;

	groups->EmplaceFast();

	groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
	groups->Back().pNext = nullptr;
	groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
	groups->Back().generalShader = 1;
	groups->Back().closestHitShader = VK_SHADER_UNUSED_NV;
	groups->Back().anyHitShader = VK_SHADER_UNUSED_NV;
	groups->Back().intersectionShader = VK_SHADER_UNUSED_NV;

	groups->EmplaceFast();

	groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
	groups->Back().pNext = nullptr;
	groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV;
	groups->Back().generalShader = VK_SHADER_UNUSED_NV;
	groups->Back().closestHitShader = 2;
	groups->Back().anyHitShader = VK_SHADER_UNUSED_NV;
	groups->Back().intersectionShader = VK_SHADER_UNUSED_NV;
}

/*
*	Creates a pipeline layout create info.
*/
void VulkanRayTracingPipeline::CreatePipelineLayoutCreateInfo(const VulkanRayTracingPipelineCreationParameters &parameters, VkPipelineLayoutCreateInfo *const RESTRICT pipelineLayoutCreateInfo) const NOEXCEPT
{
	pipelineLayoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo->pNext = nullptr;
	pipelineLayoutCreateInfo->flags = 0;
	pipelineLayoutCreateInfo->setLayoutCount = parameters._DescriptorSetLayoutCount;
	pipelineLayoutCreateInfo->pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout *RESTRICT>(parameters._DescriptorSetLayouts);
	pipelineLayoutCreateInfo->pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo->pPushConstantRanges = nullptr;
}

/*
*	Creates the ray tracing pipeline create info.
*/
void VulkanRayTracingPipeline::CreateRayTracingPipelineCreateInfo(	const ArrayProxy<VkPipelineShaderStageCreateInfo> &stages,
																	const ArrayProxy<VkRayTracingShaderGroupCreateInfoNV> &groups,
																	const VkPipelineLayout &layout,
																	VkRayTracingPipelineCreateInfoNV *const RESTRICT rayTracingPipelineCreateInfo) const NOEXCEPT
{
	rayTracingPipelineCreateInfo->sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV;
	rayTracingPipelineCreateInfo->pNext = nullptr;
	rayTracingPipelineCreateInfo->flags = 0;
	rayTracingPipelineCreateInfo->stageCount = static_cast<uint32>(stages._Size);
	rayTracingPipelineCreateInfo->pStages = stages._Array;
	rayTracingPipelineCreateInfo->groupCount = static_cast<uint32>(groups._Size);
	rayTracingPipelineCreateInfo->pGroups = groups._Array;
	rayTracingPipelineCreateInfo->maxRecursionDepth = 0;
	rayTracingPipelineCreateInfo->layout = layout;
	rayTracingPipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	rayTracingPipelineCreateInfo->basePipelineIndex = -1;
}