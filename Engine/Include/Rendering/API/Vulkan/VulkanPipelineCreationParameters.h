#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanShaderModule.h>

class VulkanPipelineCreationParameters final
{

public:

	//Whether or not to enable blend.
	bool _BlendEnable;

	//The number of color attachments.
	uint32 _ColorAttachmentCount;

	//The cull mode.
	VkCullModeFlagBits _CullMode;

	//The depth compare operator.
	VkCompareOp _DepthCompareOp;

	//Defines whether or not to enable depth test.
	VkBool32 _DepthTestEnable;

	//Defines whether or not to enable depth write.
	VkBool32 _DepthWriteEnable;

	//The descriptor set layout count.
	uint32 _DescriptorSetLayoutCount;

	//The descriptor set layouts count.
	const VulkanDescriptorSetLayout *RESTRICT _DescriptorSetLayouts;

	//The number of push constant ranges.
	uint32 _PushConstantRangeCount;

	//The push constant ranges.
	VkPushConstantRange *RESTRICT _PushConstantRanges;

	//The shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> _ShaderModules;

	//The subpass.
	uint32 _Subpass;

	//The topology of the pipeline.
	VkPrimitiveTopology _Topology;

	//The number of vertex input attribute descriptions.
	uint32 _VertexInputAttributeDescriptionCount;

	//The vertex input attribute descriptions.
	VkVertexInputAttributeDescription *RESTRICT _VertexInputAttributeDescriptions;

	//The vertex input binding description count.
	uint32 _VertexInputBindingDescriptionCount;

	//The vertex input binding descriptions.
	VkVertexInputBindingDescription *RESTRICT _VertexInputBindingDescriptions;

	//The viewport extent.
	VkExtent2D _ViewportExtent{ 0, 0 };

	//Optional render pass.
	const VulkanRenderPass *RESTRICT _RenderPass{ nullptr };

};