#if defined(CATALYST_RENDERING_VULKAN)
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
	ASSERT(_VulkanPipeline, "Double deletion detected!");

	if (_VulkanPipeline)
	{
		vkDestroyPipeline(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanPipeline, nullptr);
		_VulkanPipeline = VK_NULL_HANDLE;
	}
}

/*
*	Creates the stages.
*/
void VulkanRayTracingPipeline::CreateStages(const VulkanRayTracingPipelineCreationParameters& parameters, DynamicArray<VkPipelineShaderStageCreateInfo>* const RESTRICT stages) const NOEXCEPT
{
	//Reserve the appropriate size.
	stages->Reserve(1 + parameters._HitGroups.Size() + parameters._MissShaderModules.Size());

	//Add the ray generation shader stage.
	{
		stages->Emplace();

		VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = 0;
		pipelineShaderStageCreateInfo.stage = parameters._RayGenerationShaderModule->GetStage();
		pipelineShaderStageCreateInfo.module = parameters._RayGenerationShaderModule->Get();
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
	}

	//Add the hit group shader stages.
	for (const VulkanRayTracingPipelineCreationParameters::VulkanHitGroup& hit_group : parameters._HitGroups)
	{
		if (hit_group._ClosestHitShader)
		{
			stages->Emplace();

			VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

			pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo.pNext = nullptr;
			pipelineShaderStageCreateInfo.flags = 0;
			pipelineShaderStageCreateInfo.stage = hit_group._ClosestHitShader->GetStage();
			pipelineShaderStageCreateInfo.module = hit_group._ClosestHitShader->Get();
			pipelineShaderStageCreateInfo.pName = "main";
			pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
		}

		if (hit_group._AnyHitShader)
		{
			stages->Emplace();

			VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

			pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo.pNext = nullptr;
			pipelineShaderStageCreateInfo.flags = 0;
			pipelineShaderStageCreateInfo.stage = hit_group._AnyHitShader->GetStage();
			pipelineShaderStageCreateInfo.module = hit_group._AnyHitShader->Get();
			pipelineShaderStageCreateInfo.pName = "main";
			pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
		}

		if (hit_group._IntersectionShader)
		{
			stages->Emplace();

			VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

			pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageCreateInfo.pNext = nullptr;
			pipelineShaderStageCreateInfo.flags = 0;
			pipelineShaderStageCreateInfo.stage = hit_group._IntersectionShader->GetStage();
			pipelineShaderStageCreateInfo.module = hit_group._IntersectionShader->Get();
			pipelineShaderStageCreateInfo.pName = "main";
			pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
		}
	}

	//Add the miss shader stages.
	for (const VulkanShaderModule *const RESTRICT miss_shader_module : parameters._MissShaderModules)
	{
		//Create the pipeline shader stage create info.
		stages->Emplace();

		VkPipelineShaderStageCreateInfo &pipelineShaderStageCreateInfo{ stages->Back() };

		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = 0;
		pipelineShaderStageCreateInfo.stage = miss_shader_module->GetStage();
		pipelineShaderStageCreateInfo.module = miss_shader_module->Get();
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
	}
}

/*
*	Creates the groups.
*/
void VulkanRayTracingPipeline::CreateGroups(const VulkanRayTracingPipelineCreationParameters &parameters, DynamicArray<VkRayTracingShaderGroupCreateInfoNV> *const RESTRICT groups) const NOEXCEPT
{
	//Reserve the appropriate size.
	groups->Reserve(1 + parameters._HitGroups.Size() + parameters._MissShaderModules.Size());

	//Add all groups.
	uint32 shader_counter{ 0 };

	//Add the ray generation group.
	{
		groups->Emplace();

		groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		groups->Back().pNext = nullptr;
		groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
		groups->Back().generalShader = shader_counter++;
		groups->Back().closestHitShader = VK_SHADER_UNUSED_NV;
		groups->Back().anyHitShader = VK_SHADER_UNUSED_NV;
		groups->Back().intersectionShader = VK_SHADER_UNUSED_NV;
	}

	//Add the hit groups.
	for (const VulkanRayTracingPipelineCreationParameters::VulkanHitGroup &hit_group : parameters._HitGroups)
	{
		groups->Emplace();

		groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		groups->Back().pNext = nullptr;
		groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV;
		groups->Back().generalShader = VK_SHADER_UNUSED_NV;
		groups->Back().closestHitShader = hit_group._ClosestHitShader ? shader_counter++ : VK_SHADER_UNUSED_NV;
		groups->Back().anyHitShader = hit_group._AnyHitShader ? shader_counter++ : VK_SHADER_UNUSED_NV;
		groups->Back().intersectionShader = hit_group._IntersectionShader ? shader_counter++ : VK_SHADER_UNUSED_NV;
	}

	//Add the miss groups.
	for (const VulkanShaderModule *const RESTRICT miss_shader_module : parameters._MissShaderModules)
	{
		groups->Emplace();

		groups->Back().sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
		groups->Back().pNext = nullptr;
		groups->Back().type = VkRayTracingShaderGroupTypeNV::VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
		groups->Back().generalShader = shader_counter++;
		groups->Back().closestHitShader = VK_SHADER_UNUSED_NV;
		groups->Back().anyHitShader = VK_SHADER_UNUSED_NV;
		groups->Back().intersectionShader = VK_SHADER_UNUSED_NV;
	}
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
	pipelineLayoutCreateInfo->pushConstantRangeCount = parameters._PushConstantRangeCount;
	pipelineLayoutCreateInfo->pPushConstantRanges = parameters._PushConstantRanges;
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
	rayTracingPipelineCreateInfo->maxRecursionDepth = 3;
	rayTracingPipelineCreateInfo->layout = layout;
	rayTracingPipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	rayTracingPipelineCreateInfo->basePipelineIndex = -1;
}
#endif