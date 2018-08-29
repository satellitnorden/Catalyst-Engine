#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanShaderModule.h>

class VulkanPipelineCreationParameters final
{

public:

	//Whether or not to enable blend.
	bool blendEnable;

	//The number of color attachments.
	uint32 colorAttachmentCount;

	//The cull mode.
	VkCullModeFlagBits cullMode;

	//The depth compare operator.
	VkCompareOp depthCompareOp;

	//Defines whether or not to enable depth test.
	VkBool32 depthTestEnable;

	//Defines whether or not to enable depth write.
	VkBool32 depthWriteEnable;

	//The descriptor set layout count.
	uint32 descriptorSetLayoutCount;

	//The descriptor set layouts count.
	const VulkanDescriptorSetLayout *RESTRICT descriptorSetLayouts;

	//The number of push constant ranges.
	uint32 pushConstantRangeCount;

	//The push constant ranges.
	VkPushConstantRange *RESTRICT pushConstantRanges;

	//The shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> shaderModules;

	//The subpass.
	uint32 subpass;

	//The topology of the pipeline.
	VkPrimitiveTopology topology;

	//The number of vertex input attribute descriptions.
	uint32 vertexInputAttributeDescriptionCount;

	//The vertex input attribute descriptions.
	VkVertexInputAttributeDescription *RESTRICT vertexInputAttributeDescriptions;

	//The vertex input binding description count.
	uint32 vertexInputBindingDescriptionCount;

	//The vertex input binding descriptions.
	VkVertexInputBindingDescription *RESTRICT vertexInputBindingDescriptions;

	//The viewport extent.
	VkExtent2D viewportExtent{ 0, 0 };

	//Optional render pass.
	const VulkanRenderPass *RESTRICT renderPass{ nullptr };

};