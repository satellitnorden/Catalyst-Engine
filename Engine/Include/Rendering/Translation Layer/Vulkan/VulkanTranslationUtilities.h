#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalVertex.h>
#include <Rendering/Engine Layer/TextureData.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanTranslationUtilities
{

public:

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
				PRINT_TO_CONSOLE("Unknown address mode."); BREAKPOINT;
#endif
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
		}
	}

	/*
	*	Given an attachment load operator, return the corresponding Vulkan attachment load operator.
	*/
	static VkAttachmentLoadOp GetVulkanAttachmentLoadOperator(const AttachmentLoadOperator attachmentLoadOperator) NOEXCEPT
	{
		switch (attachmentLoadOperator)
		{
			default: return VK_ATTACHMENT_LOAD_OP_LOAD;
			case AttachmentLoadOperator::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case AttachmentLoadOperator::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case AttachmentLoadOperator::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
		}
	}

	/*
	*	Given an attachment store operator, return the corresponding Vulkan attachment load operator.
	*/
	static VkAttachmentStoreOp GetVulkanAttachmentStoreOperator(const AttachmentStoreOperator attachmentSstoreOperator) NOEXCEPT
	{
		switch (attachmentSstoreOperator)
		{
			default: return VK_ATTACHMENT_STORE_OP_STORE;
			case AttachmentStoreOperator::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case AttachmentStoreOperator::Store: return VK_ATTACHMENT_STORE_OP_STORE;
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
				PRINT_TO_CONSOLE("Unknown texture format."); BREAKPOINT;
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
				PRINT_TO_CONSOLE("Unknown mipmap mode."); BREAKPOINT;
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
		return VkPushConstantRange{ GetVulkanShaderStage(pushConstantRange.shaderStage),
									pushConstantRange.offset,
									pushConstantRange.size };
	}

	/*
	*	Given a shader stage, returns the corresponding Vulkan shader stage.
	*/
	static VkShaderStageFlags GetVulkanShaderStage(const PushConstantRange::ShaderStage shaderStage) NOEXCEPT
	{
		switch (shaderStage)
		{
			default: return VK_SHADER_STAGE_VERTEX_BIT;
			case PushConstantRange::ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
			case PushConstantRange::ShaderStage::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case PushConstantRange::ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case PushConstantRange::ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
			case PushConstantRange::ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case PushConstantRange::ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
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
				PRINT_TO_CONSOLE("Unknown texture filter."); BREAKPOINT;
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
		return VkVertexInputAttributeDescription{	vertexInputAttributeDescription.location,
													vertexInputAttributeDescription.binding,
													GetVulkanFormat(vertexInputAttributeDescription.format),
													vertexInputAttributeDescription.offset };
	}

	/*
	*	Given a vertex input binding description, returns the corresponding Vulkan vertex input binding description.
	*/
	static VkVertexInputBindingDescription GetVulkanVertexInputBindingDescription(const VertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		return VkVertexInputBindingDescription{	vertexInputBindingDescription.binding,
												vertexInputBindingDescription.stride,
												GetVulkanInputRate(vertexInputBindingDescription.inputRate) };
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