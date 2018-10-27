#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanTranslationUtilities
{

public:

	/*
	*	Given a main stage and a sub stage, return the corresponding sub stage index.
	*/
	static uint8 GetSubStageIndex(const RenderPassMainStage main, const RenderPassSubStage sub) NOEXCEPT
	{
		switch (main)
		{
#if !defined(CATALYST_FINAL)
			case RenderPassMainStage::None:
			{
				ASSERT(false, "None main stage, how could this be?");

				return 0;
			}
#endif
			case RenderPassMainStage::DirectionalShadow:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::DirectionalTerrainShadow);
			}

			case RenderPassMainStage::Scene:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::TerrainDepth);
			}
#if !defined(CATALYST_FINAL)
			case RenderPassMainStage::Debug:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::DebugAxisAlignedBoundingBox);
			}
#endif
#if defined(CATALYST_ENABLE_OCEAN)
			case RenderPassMainStage::Ocean:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::AboveOcean);
			}
#endif
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
			case RenderPassMainStage::VolumetricFog:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::VolumetricFog);
			}
#endif
			case RenderPassMainStage::DepthOfFieldHorizontal:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::DepthOfFieldHorizontal);
			}

			case RenderPassMainStage::DepthOfFieldVertical:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::DepthOfFieldVertical);
			}

			case RenderPassMainStage::ToneMapping:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::ToneMapping);
			}

			case RenderPassMainStage::AntiAliasing:
			{
				return UNDERLYING(sub) - UNDERLYING(RenderPassSubStage::AntiAliasing);
			}
#if !defined(CATALYST_FINAL)
			default:
			{
				ASSERT(false, "Unhandled case.");

				return 0;
			}
#endif
		}
	}

	/*
	*	Given an address mode, return the corresponding Vulkan address mode.
	*/
	static VkSamplerAddressMode GetVulkanAddressMode(const AddressMode addressMode) NOEXCEPT
	{
		switch (addressMode)
		{
			case AddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case AddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case AddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			case AddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case AddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;

			default:
			{
#if !defined(CATALYST_FINAL)
				ASSERT(false, "Unknown address mode.");
#endif
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
		}
	}

	/*
	*	Given a compare operator, return the corresponding Vulkan compare operator.
	*/
	static VkCompareOp GetVulkanCompareOperator(const CompareOperator compareOperator) NOEXCEPT
	{
		switch (compareOperator)
		{
			default: return VK_COMPARE_OP_NEVER;
			case CompareOperator::Always: return VK_COMPARE_OP_ALWAYS;
			case CompareOperator::Equal: return VK_COMPARE_OP_EQUAL;
			case CompareOperator::Greater: return VK_COMPARE_OP_GREATER;
			case CompareOperator::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case CompareOperator::Less: return VK_COMPARE_OP_LESS;
			case CompareOperator::LessOrEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
			case CompareOperator::Never: return VK_COMPARE_OP_NEVER;
			case CompareOperator::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
		}
	}

	/*
	*	Given a cull mode, return the corresponding Vulkan cull mode.
	*/
	static VkCullModeFlagBits GetVulkanCullMode(const CullMode cullMode) NOEXCEPT
	{
		switch (cullMode)
		{
			default: return VK_CULL_MODE_NONE;
			case CullMode::None: return VK_CULL_MODE_NONE;
			case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
			case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
			case CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
		}
	}

	/*
	*	Given a render data table layout binding type, return the corresponding Vulkan descriptor type.
	*/
	static VkDescriptorType GetVulkanDescriptorType(const RenderDataTableLayoutBinding::Type type) NOEXCEPT
	{
		switch (type)
		{
			case RenderDataTableLayoutBinding::Type::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case RenderDataTableLayoutBinding::Type::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			default:
			{
#if !defined(CATALYST_FINAL)
				ASSERT(false, "Unknown texture format.");
#endif
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			}
		}
	}

	/*
	*	Given a resolution, return the corresponding Vulkan extent.
	*/
	static VkExtent2D GetVulkanExtent(const Resolution resolution) NOEXCEPT
	{
		return VkExtent2D{ resolution._Width, resolution._Height };
	}

	/*
	*	Given a format, return the corresponding Vulkan format.
	*/
	static VkFormat GetVulkanFormat(const TextureFormat format) NOEXCEPT
	{
		switch (format)
		{
			case TextureFormat::R8_Byte: return VK_FORMAT_R8_UNORM;
			case TextureFormat::R8G8B8A8_Byte: return VK_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::R32G32B32A32_Float: return VK_FORMAT_R32G32B32A32_SFLOAT;

			default:
			{
#if !defined(CATALYST_FINAL)
				ASSERT(false, "Unknown texture format.");
#endif
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
		}
	}

	/*
	*	Given a format, return the corresponding Vulkan format.
	*/
	static VkFormat GetVulkanFormat(const VertexInputAttributeDescription::Format format) NOEXCEPT
	{
		switch (format)
		{
			default: return VK_FORMAT_UNDEFINED;
			case VertexInputAttributeDescription::Format::X32SignedFloat: return VK_FORMAT_R32_SFLOAT;
			case VertexInputAttributeDescription::Format::X32Y32SignedFloat: return VK_FORMAT_R32G32_SFLOAT;
			case VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat: return VK_FORMAT_R32G32B32_SFLOAT;
			case VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	}

	/*
	*	Given a mipmap mode, return the corresponding Vulkan mipmap mode.
	*/
	static VkSamplerMipmapMode GetVulkanMipmapMode(const MipmapMode mipmapMode) NOEXCEPT
	{
		switch (mipmapMode)
		{
			case MipmapMode::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			case MipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			default:
			{
#if !defined(CATALYST_FINAL)
				ASSERT(false, "Unknown mipmap mode.");
#endif
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			}
		}
	}

	/*
	*	Given a push constant range, returns the corresponding Vulkan push constant range.
	*/
	static VkPushConstantRange GetVulkanPushConstantRange(const PushConstantRange &pushConstantRange) NOEXCEPT
	{
		return VkPushConstantRange{ GetVulkanShaderStages(pushConstantRange.shaderStage),
									pushConstantRange.offset,
									pushConstantRange.size };
	}

	/*
	*	Given a shader stage, returns the corresponding Vulkan shader stage.
	*/
	static VkShaderStageFlags GetVulkanShaderStages(const ShaderStage shaderStage) NOEXCEPT
	{
		VkShaderStageFlags flags{ 0 };

		if (IS_BIT_SET(shaderStage, ShaderStage::Vertex))
		{
			flags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (IS_BIT_SET(shaderStage, ShaderStage::TessellationControl))
		{
			flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}

		if (IS_BIT_SET(shaderStage, ShaderStage::TessellationEvaluation))
		{
			flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		if (IS_BIT_SET(shaderStage, ShaderStage::Geometry))
		{
			flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}

		if (IS_BIT_SET(shaderStage, ShaderStage::Fragment))
		{
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (IS_BIT_SET(shaderStage, ShaderStage::Compute))
		{
			flags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		return flags;
	}

	/*
	*	Given a stencil operator, return the corresponding stencil compare operator.
	*/
	static VkStencilOp GetVulkanStencilOperator(const StencilOperator stencilOperator) NOEXCEPT
	{
		switch (stencilOperator)
		{
			default: return VK_STENCIL_OP_KEEP;
			case StencilOperator::Keep: return VK_STENCIL_OP_KEEP;
			case StencilOperator::Zero: return VK_STENCIL_OP_ZERO;
			case StencilOperator::Replace: return VK_STENCIL_OP_REPLACE;
			case StencilOperator::IncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case StencilOperator::DecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case StencilOperator::Invert: return VK_STENCIL_OP_INVERT;
			case StencilOperator::IncrementAndWrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case StencilOperator::DecrementAndWrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
	}

	/*
	*	Given a texture filter, return the corresponding Vulkan texture filter.
	*/
	static VkFilter GetVulkanTextureFilter(const TextureFilter textureFilter) NOEXCEPT
	{
		switch (textureFilter)
		{
			case TextureFilter::Linear: return VK_FILTER_LINEAR;
			case TextureFilter::Nearest: return VK_FILTER_NEAREST;

			default:
			{
#if !defined(CATALYST_FINAL)
				ASSERT(false, "Unknown texture filter.");
#endif
				return VK_FILTER_NEAREST;
			}
		}
	}

	/*
	*	Given a topology, returns the corresponding Vulkan topology.
	*/
	static VkPrimitiveTopology GetVulkanTopology(const Topology topology) NOEXCEPT
	{
		switch (topology)
		{
			default: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case Topology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case Topology::LineListWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
			case Topology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case Topology::LineStripWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
			case Topology::PatchList: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			case Topology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case Topology::TriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			case Topology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case Topology::TriangleListWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
			case Topology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case Topology::TriangleStripWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		}
	}

	/*
	*	Given a vertex input attribute description, returns the corresponding Vulkan vertex input attribute description.
	*/
	static VkVertexInputAttributeDescription GetVulkanVertexInputAttributeDescription(const VertexInputAttributeDescription &vertexInputAttributeDescription) NOEXCEPT
	{
		return VkVertexInputAttributeDescription{	vertexInputAttributeDescription._Location,
													vertexInputAttributeDescription._Binding,
													GetVulkanFormat(vertexInputAttributeDescription._Format),
													vertexInputAttributeDescription._Offset };
	}

	/*
	*	Given a vertex input binding description, returns the corresponding Vulkan vertex input binding description.
	*/
	static VkVertexInputBindingDescription GetVulkanVertexInputBindingDescription(const VertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		return VkVertexInputBindingDescription{	vertexInputBindingDescription._Binding,
												vertexInputBindingDescription._Stride,
												GetVulkanInputRate(vertexInputBindingDescription._InputRate) };
	}

	/*
	*	Given an input rate, returns the corresponding Vulkan input rate.
	*/
	static VkVertexInputRate GetVulkanInputRate(const VertexInputBindingDescription::InputRate inputRate) NOEXCEPT
	{
		switch (inputRate)
		{
			default: return VK_VERTEX_INPUT_RATE_VERTEX;
			case VertexInputBindingDescription::InputRate::Instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
			case VertexInputBindingDescription::InputRate::Vertex: return VK_VERTEX_INPUT_RATE_VERTEX;
		}
	}

};