#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanShaderModule.h>

class DescriptorLayoutBindingInformation
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	DescriptorLayoutBindingInformation() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	DescriptorLayoutBindingInformation(const uint32 newBinding, const VkDescriptorType newDescriptorType, const VkShaderStageFlags newShaderStage) CATALYST_NOEXCEPT
		:
		binding(newBinding),
		descriptorType(newDescriptorType),
		shaderStages(newShaderStage)
	{

	}

	uint32 binding{ 0 };
	VkDescriptorType descriptorType{ VK_DESCRIPTOR_TYPE_MAX_ENUM };
	VkShaderStageFlags shaderStages{ VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM };
};

class VulkanPipelineCreationParameters
{

public:

	//The shader modules.
	DynamicArray<VulkanShaderModule *CATALYST_RESTRICT> shaderModules{ };

	//The viewport extent.
	VkExtent2D viewportExtent{ 0, 0 };

	//The descriptor layout binding informations.
	DynamicArray<DescriptorLayoutBindingInformation> descriptorLayoutBindingInformations{ };

	//The color attachments.
	DynamicArray<DynamicArray<VkImageView>> colorAttachments{ };

};