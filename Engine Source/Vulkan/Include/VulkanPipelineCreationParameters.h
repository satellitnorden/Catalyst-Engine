#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanShaderModule.h>

class DescriptorLayoutBindingInformation final
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

class VulkanPipelineCreationParameters final
{

public:

	//The attachment load operator.
	VkAttachmentLoadOp attachmentLoadOperator;

	//The final layout that color attachments will be in when a render pass instance begins.
	VkImageLayout colorAttachmentFinalLayout;

	//The format of the color attachments.
	VkFormat colorAttachmentFormat;
	
	//The initial layout that color attachments will be in when a render pass instance begins.
	VkImageLayout colorAttachmentInitialLayout;

	//The color attachments.
	DynamicArray<DynamicArray<VkImageView>> colorAttachments{};

	//The final layout that depth attachments will be in when a render pass instance begins.
	VkImageLayout depthAttachmentFinalLayout;

	//The initial layout that depth attachments will be in when a render pass instance begins.
	VkImageLayout depthAttachmentInitialLayout;

	//The depth buffers.
	DynamicArray<const VulkanDepthBuffer *CATALYST_RESTRICT> depthBuffers{};

	//The depth compare operator.
	VkCompareOp depthCompareOp;

	//Defines whether or not to enable depth test.
	VkBool32 depthTestEnable;

	//Defines whether or not to enable depth write.
	VkBool32 depthWriteEnable;

	//The descriptor layout binding informations.
	DynamicArray<DescriptorLayoutBindingInformation> descriptorLayoutBindingInformations{};

	//The shader modules.
	DynamicArray<VulkanShaderModule *CATALYST_RESTRICT> shaderModules{};

	//The topology of the pipeline.
	VkPrimitiveTopology topology;

	//The viewport extent.
	VkExtent2D viewportExtent{ 0, 0 };

};