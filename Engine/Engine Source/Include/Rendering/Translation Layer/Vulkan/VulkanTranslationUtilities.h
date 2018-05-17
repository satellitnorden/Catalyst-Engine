#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalVertex.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/VegetationTransformation.h>

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
			case VertexInputAttributeDescription::Format::X32Y32SignedFloat: return VK_FORMAT_R32G32_SFLOAT;
			case VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat: return VK_FORMAT_R32G32B32_SFLOAT;
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

	/*
	*	Returns the vertex input attribute descriptions for the instanced physical pipeline.
	*/
	static void GetInstancedPhysicalVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 8> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = offsetof(PhysicalVertex, position);

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = offsetof(PhysicalVertex, normal);

		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[2].offset = offsetof(PhysicalVertex, tangent);

		vertexInputAttributeDescriptions[3].location = 3;
		vertexInputAttributeDescriptions[3].binding = 0;
		vertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[3].offset = offsetof(PhysicalVertex, textureCoordinate);

		vertexInputAttributeDescriptions[4].location = 4;
		vertexInputAttributeDescriptions[4].binding = 1;
		vertexInputAttributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescriptions[4].offset = 0;

		vertexInputAttributeDescriptions[5].location = 5;
		vertexInputAttributeDescriptions[5].binding = 1;
		vertexInputAttributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescriptions[5].offset = sizeof(Vector4);

		vertexInputAttributeDescriptions[6].location = 6;
		vertexInputAttributeDescriptions[6].binding = 1;
		vertexInputAttributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescriptions[6].offset = sizeof(Vector4) * 2;

		vertexInputAttributeDescriptions[7].location = 7;
		vertexInputAttributeDescriptions[7].binding = 1;
		vertexInputAttributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescriptions[7].offset = sizeof(Vector4) * 3;
	}

	/*
	*	Returns the vertex input binding description for the physical entity scene buffer render pass.
	*/
	static void GetInstancedPhysicalVertexInputBindingDescriptions(StaticArray<VkVertexInputBindingDescription, 2> &vertexInputBindingDescriptions) NOEXCEPT
	{
		vertexInputBindingDescriptions[0].binding = 0;
		vertexInputBindingDescriptions[0].stride = sizeof(PhysicalVertex);
		vertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		vertexInputBindingDescriptions[1].binding = 1;
		vertexInputBindingDescriptions[1].stride = sizeof(Matrix4);
		vertexInputBindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
	}

	/*
	*	Returns the vertex input attribute descriptions for the physical entity scene buffer render pass.
	*/
	static void GetPhysicalVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 4> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = offsetof(PhysicalVertex, position);

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = offsetof(PhysicalVertex, normal);

		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[2].offset = offsetof(PhysicalVertex, tangent);

		vertexInputAttributeDescriptions[3].location = 3;
		vertexInputAttributeDescriptions[3].binding = 0;
		vertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[3].offset = offsetof(PhysicalVertex, textureCoordinate);
	}

	/*
	*	Returns the vertex input binding description for the physical entity scene buffer render pass.
	*/
	static void GetPhysicalVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(PhysicalVertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	/*
	*	Returns the vertex input attribute descriptions for the terrain entity scene buffer render pass.
	*/
	static void GetTerrainVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 2> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = 0;

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;
	}

	/*
	*	Returns the vertex input binding description for the terrain entity scene buffer render pass.
	*/
	static void GetTerrainVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(float) * 5;
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	/*
	*	Returns the vertex input attribute descriptions for the vegetation pipeline.
	*/
	static void GetVegetationVertexInputAttributeDescriptions(StaticArray<VkVertexInputAttributeDescription, 3> &vertexInputAttributeDescriptions) NOEXCEPT
	{
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = 0;

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = sizeof(Vector3);

		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32_SFLOAT;
		vertexInputAttributeDescriptions[2].offset = sizeof(Vector3) + sizeof(Vector2);
	}

	/*
	*	Returns the vertex input binding description for the vegetation pipeline.
	*/
	static void GetVegetationVertexInputBindingDescription(VkVertexInputBindingDescription &vertexInputBindingDescription) NOEXCEPT
	{
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(VegetationTransformation);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
	}

};