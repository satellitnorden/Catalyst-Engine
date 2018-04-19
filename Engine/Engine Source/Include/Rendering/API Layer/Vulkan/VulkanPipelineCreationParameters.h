#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>
#include <Rendering/API Layer/Vulkan/VulkanShaderModule.h>

class VulkanPipelineCreationParameters final
{

public:

	//The attachment load operator.
	VkAttachmentLoadOp attachmentLoadOperator;

	//Whether or not to enable blend.
	bool blendEnable;

	//The final layout that color attachments will be in when a render pass instance begins.
	VkImageLayout colorAttachmentFinalLayout;

	//The format of the color attachments.
	VkFormat colorAttachmentFormat;
	
	//The initial layout that color attachments will be in when a render pass instance begins.
	VkImageLayout colorAttachmentInitialLayout;

	//The color attachments.
	DynamicArray<DynamicArray<VkImageView>> colorAttachments{ };

	//The cull mode.
	VkCullModeFlagBits cullMode;

	//The final layout that depth attachments will be in when a render pass instance begins.
	VkImageLayout depthAttachmentFinalLayout;

	//The initial layout that depth attachments will be in when a render pass instance begins.
	VkImageLayout depthAttachmentInitialLayout;

	//The store op for the depth attachment.
	VkAttachmentStoreOp depthAttachmentStoreOp;

	//The depth buffer.
	VulkanDepthBuffer *RESTRICT depthBuffer;

	//The depth compare operator.
	VkCompareOp depthCompareOp;

	//Defines whether or not to enable depth test.
	VkBool32 depthTestEnable;

	//Defines whether or not to enable depth write.
	VkBool32 depthWriteEnable;

	//The descriptor set layout count.
	uint32 descriptorSetLayoutCount;

	//The descriptor set layouts count.
	VulkanDescriptorSetLayout *RESTRICT descriptorSetLayouts;

	//The number of push constant ranges.
	uint32 pushConstantRangeCount;

	//The push constant ranges.
	VkPushConstantRange *RESTRICT pushConstantRanges;

	//The shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> shaderModules{};

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

};