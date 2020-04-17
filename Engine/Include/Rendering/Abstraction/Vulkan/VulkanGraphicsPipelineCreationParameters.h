#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>

class VulkanGraphicsPipelineCreationParameters final
{

public:

	//Whether or not to enable blend.
	bool _BlendEnable;

	//The blend factor for the source color.
	VkBlendFactor _BlendFactorSourceColor;

	//The blend factor for the destination color.
	VkBlendFactor _BlendFactorDestinationColor;

	//The blend factor for the source alpha.
	VkBlendFactor _BlendFactorSourceAlpha;

	//The blend factor for the destination alpha.
	VkBlendFactor _BlendFactorDestinationAlpha;

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
	DynamicArray<const VulkanShaderModule *RESTRICT> _ShaderModules;

	//Defines whether or not to enable stencil test.
	VkBool32 _StencilTestEnable;

	//The stencil fail operator.
	VkStencilOp _StencilFailOperator;

	//The stencil pass operator.
	VkStencilOp _StencilPassOperator;

	//The stencil depth fail operator.
	VkStencilOp _StencilDepthFailOperator;

	//The stencil compare operator.
	VkCompareOp _StencilCompareOperator;

	//The stencil compare mask.
	uint32 _StencilCompareMask;

	//The stencil write mask.
	uint32 _StencilWriteMask;

	//The stencil reference mask.
	uint32 _StencilReferenceMask;

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
#endif