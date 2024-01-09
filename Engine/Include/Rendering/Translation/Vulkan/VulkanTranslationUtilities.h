#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanGeometryInstance.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

//Systems.
#include <Systems/RenderingSystem.h>

class VulkanTranslationUtilities
{

public:

	/*
	*	Given adress flags, return the corresponding Vulkan access flags.
	*/
	static VkAccessFlags GetVulkanAccessFlags(const AccessFlags access_flags) NOEXCEPT
	{
		return 0;
	}

	/*
	*	Given an address mode, return the corresponding Vulkan address mode.
	*/
	static VkSamplerAddressMode GetVulkanAddressMode(const AddressMode addressMode) NOEXCEPT
	{
		switch (addressMode)
		{
			case AddressMode::CLAMP_TO_BORDER: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case AddressMode::CLAMP_TO_EDGE: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case AddressMode::MIRROR_CLAMP_TO_EDGE: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			case AddressMode::MIRRORED_REPEAT: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case AddressMode::REPEAT: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown address mode.");
#endif
				return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
		}
	}

	/*
	*	Given an attachment load operator, returns the corresponding Vulkan attachment load operator.
	*/
	FORCE_INLINE static VkAttachmentLoadOp GetVulkanAttachmentLoadOperator(const AttachmentLoadOperator value) NOEXCEPT
	{
		switch (value)
		{
			case AttachmentLoadOperator::LOAD:
			{
				return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
			}

			case AttachmentLoadOperator::CLEAR:
			{
				return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
			}

			case AttachmentLoadOperator::DONT_CARE:
			{
				return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			}
		}
	}

	/*
	*	Given an attachment store operator, returns the corresponding Vulkan attachment store operator.
	*/
	FORCE_INLINE static VkAttachmentStoreOp GetVulkanAttachmentStoreOperator(const AttachmentStoreOperator value) NOEXCEPT
	{
		switch (value)
		{
			case AttachmentStoreOperator::STORE:
			{
				return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
			}

			case AttachmentStoreOperator::DONT_CARE:
			{
				return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
		}
	}

	/*
	*	Given a blend factor, return the corresponding Vulkan blend factor.
	*/
	static VkBlendFactor GetVulkanBlendFactor(const BlendFactor blendFactor) NOEXCEPT
	{
		switch (blendFactor)
		{
			case BlendFactor::Zero: return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
			case BlendFactor::One: return VkBlendFactor::VK_BLEND_FACTOR_ONE;
			case BlendFactor::SourceColor: return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
			case BlendFactor::OneMinusSourceColor: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case BlendFactor::DestinationColor: return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
			case BlendFactor::OneMinusDestinationColor: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case BlendFactor::SourceAlpha: return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
			case BlendFactor::OneMinusSourceAlpha: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case BlendFactor::DestinationAlpha: return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
			case BlendFactor::OneMinusDestinationAlpha: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown blend factor.");
#endif
				return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
			}
		}
	}

	/*
	*	Given a blend operator, return the corresponding Vulkan blend operator.
	*/
	static VkBlendOp GetVulkanBlendOperator(const BlendOperator blend_operator) NOEXCEPT
	{
		switch (blend_operator)
		{
			case BlendOperator::ADD: return VkBlendOp::VK_BLEND_OP_ADD;
			case BlendOperator::SUBTRACT: return VkBlendOp::VK_BLEND_OP_SUBTRACT;
			case BlendOperator::REVERSE_SUBTRACT: return VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;
			case BlendOperator::MIN: return VkBlendOp::VK_BLEND_OP_MIN;
			case BlendOperator::MAX: return VkBlendOp::VK_BLEND_OP_MAX;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown blend operator.");
#endif
				return VkBlendOp::VK_BLEND_OP_ADD;
			}
		}
	}

	/*
	*	Given a buffer usage, returns the corresponding Vulkan buffer usage.
	*/
	static VkBufferUsageFlags GetVulkanBufferUsage(const BufferUsage usage) NOEXCEPT
	{
		VkBufferUsageFlags flags{ 0 };

#define MAPPING(USAGE, BIT) if (TEST_BIT(usage, USAGE)) flags |= BIT;

		MAPPING(BufferUsage::IndexBuffer, VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		MAPPING(BufferUsage::StorageBuffer, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(BufferUsage::RayTracing, VkBufferUsageFlagBits::VK_BUFFER_USAGE_RAY_TRACING_BIT_NV);
		MAPPING(BufferUsage::UniformBuffer, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		MAPPING(BufferUsage::VertexBuffer, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

#undef MAPPING

		return flags;
	}

	/*
	*	Given a compare operator, return the corresponding Vulkan compare operator.
	*/
	static VkCompareOp GetVulkanCompareOperator(const CompareOperator compareOperator) NOEXCEPT
	{
		switch (compareOperator)
		{
			default: return VK_COMPARE_OP_NEVER;
			case CompareOperator::Always: return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			case CompareOperator::Equal: return VkCompareOp::VK_COMPARE_OP_EQUAL;
			case CompareOperator::Greater: return VkCompareOp::VK_COMPARE_OP_GREATER;
			case CompareOperator::GreaterOrEqual: return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
			case CompareOperator::Less: return VkCompareOp::VK_COMPARE_OP_LESS;
			case CompareOperator::LessOrEqual: return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
			case CompareOperator::Never: return VkCompareOp::VK_COMPARE_OP_NEVER;
			case CompareOperator::NotEqual: return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
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
			case RenderDataTableLayoutBinding::Type::AccelerationStructure: if (RenderingSystem::Instance->IsRayTracingSupported()) return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV; else return static_cast<VkDescriptorType>(0);
			case RenderDataTableLayoutBinding::Type::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case RenderDataTableLayoutBinding::Type::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case RenderDataTableLayoutBinding::Type::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
			case RenderDataTableLayoutBinding::Type::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case RenderDataTableLayoutBinding::Type::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case RenderDataTableLayoutBinding::Type::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
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
			case TextureFormat::R_UINT8: return VkFormat::VK_FORMAT_R8_UNORM;
			case TextureFormat::R_UINT16: return VkFormat::VK_FORMAT_R16_UNORM;
			case TextureFormat::RGB_UINT8: return VkFormat::VK_FORMAT_R8G8B8_UNORM;
			case TextureFormat::RGBA_UINT8: return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::RG_FLOAT16: return VkFormat::VK_FORMAT_R16G16_SFLOAT;
			case TextureFormat::R_FLOAT32: return VkFormat::VK_FORMAT_R32_SFLOAT;
			case TextureFormat::RG_FLOAT32: return VkFormat::VK_FORMAT_R32G32_SFLOAT;
			case TextureFormat::RGBA_FLOAT32: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			case TextureFormat::D_UINT24_S_UINT8: return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::D_UINT16: return VkFormat::VK_FORMAT_D16_UNORM;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
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
			case VertexInputAttributeDescription::Format::X32SignedInt: return VK_FORMAT_R32_SINT;
			case VertexInputAttributeDescription::Format::X32UnsignedInt: return VK_FORMAT_R32_UINT;
			case VertexInputAttributeDescription::Format::X32Y32Z32SignedInt: return VK_FORMAT_R32G32B32_SINT;
		}
	}

	/*
	*	Give bottom level acceleration structure flags, returns the corresponding Vulkan geometry flags.
	*/
	static VkGeometryFlagsKHR GetVulkanGeometryFlags(const BottomLevelAccelerationStructureFlag flags) NOEXCEPT
	{
		VkBufferUsageFlags out_flags{ 0 };

#define MAPPING(FLAG, BIT) if (TEST_BIT(flags, FLAG)) out_flags |= BIT;

		MAPPING(BottomLevelAccelerationStructureFlag::OPAQUE, VkGeometryFlagBitsKHR::VK_GEOMETRY_OPAQUE_BIT_KHR);

#undef MAPPING

		return out_flags;
	}

	/*
	*	Given a top level acceleration structure instance data, convert it to a Vulkan geometry instance.
	*/
	static void GetVulkanGeometryInstance(const TopLevelAccelerationStructureInstanceData &data, VulkanGeometryInstance *const RESTRICT geometryInstance)
	{
		//Transpose and copy the transform.
		Matrix4x4 transposedTransform{ data._Transform };
		transposedTransform.Transpose();

		Memory::Copy(geometryInstance->_Transform, &transposedTransform, sizeof(geometryInstance->_Transform));

		//Set the custom index.
		geometryInstance->_InstanceCustomIndex = data._InstanceIndex;

		//Set the mask.
		geometryInstance->_Mask = UINT8_MAXIMUM;

		//Set the instance offset.
		geometryInstance->_InstanceOffset = data._HitGroupIndex;

		//Set the flags.
		geometryInstance->_Flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_NV | VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_NV;

		//Get the bottom level acceleration structure handle.
		vkGetAccelerationStructureHandleNV(	VulkanInterface::Instance->GetLogicalDevice().Get(),
											static_cast<VulkanAccelerationStructure *const RESTRICT>(data._BottomLevelAccelerationStructure)->GetAccelerationStructure(),
											sizeof(uint64),
											&geometryInstance->_BottomLevelAccelerationStructureHandle);
	}

	/*
	*	Given an image layout, return the corresponding Vulkan image layout.
	*/
	static VkImageLayout GetVulkanImageLayout(const ImageLayout image_layout) NOEXCEPT
	{
		return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	}

	/*
	*	Given a texture usage, returns the corresponding Vulkan image usage.
	*/
	static VkImageUsageFlags GetVulkanImageUsage(const TextureUsage usage) NOEXCEPT
	{
		VkImageUsageFlags flags{ 0 };

		if (usage == TextureUsage::STORAGE)
		{
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
		}

		return flags;
	}

	/*
	*	Given a memory property, returns the corresponding Vulkan memory property.
	*/
	static VkMemoryPropertyFlags GetVulkanMemoryProperty(const MemoryProperty property) NOEXCEPT
	{
		VkMemoryPropertyFlags flags{ 0 };

#define MAPPING(PROPERTY, BIT) if (TEST_BIT(property, PROPERTY)) flags |= BIT;

		MAPPING(MemoryProperty::DeviceLocal, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		MAPPING(MemoryProperty::HostCoherent, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		MAPPING(MemoryProperty::HostVisible, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

#undef MAPPING

		return flags;
	}

	/*
	*	Given a mipmap mode, return the corresponding Vulkan mipmap mode.
	*/
	static VkSamplerMipmapMode GetVulkanMipmapMode(const MipmapMode mipmapMode) NOEXCEPT
	{
		switch (mipmapMode)
		{
			case MipmapMode::LINEAR: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			case MipmapMode::NEAREST: return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown mipmap mode.");
#endif
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			}
		}
	}

	/*
	*	Given pipeline stage flags, return the corresponding Vulkan pipeline stage flags.
	*/
	static VkPipelineStageFlags GetVulkanPipelineStageFlags(const PipelineStageFlags pipeline_stage_flags) NOEXCEPT
	{
		return 0;
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
	*	Given a sample count, return the corresponding Vulkan sample count.
	*/
	static VkSampleCountFlagBits GetVulkanSampleCount(const SampleCount sample_count) NOEXCEPT
	{
		switch (sample_count)
		{
			case SampleCount::SAMPLE_COUNT_1: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			case SampleCount::SAMPLE_COUNT_2: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
			case SampleCount::SAMPLE_COUNT_4: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
			case SampleCount::SAMPLE_COUNT_8: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
			case SampleCount::SAMPLE_COUNT_16: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
			case SampleCount::SAMPLE_COUNT_32: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;
			case SampleCount::SAMPLE_COUNT_64: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown sample count.");
#endif
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			}
		}
	}

	/*
	*	Given a shader stage, returns the corresponding Vulkan shader stage.
	*/
	static VkShaderStageFlags GetVulkanShaderStages(const ShaderStage shaderStage) NOEXCEPT
	{
		VkShaderStageFlags flags{ 0 };

#define MAPPING(STAGE, BIT) if (TEST_BIT(shaderStage, STAGE)) flags |= BIT;

		MAPPING(ShaderStage::COMPUTE,				VK_SHADER_STAGE_COMPUTE_BIT);
		MAPPING(ShaderStage::FRAGMENT,				VK_SHADER_STAGE_FRAGMENT_BIT);
		MAPPING(ShaderStage::GEOMETRY,				VK_SHADER_STAGE_GEOMETRY_BIT);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(ShaderStage::RAY_ANY_HIT,		VK_SHADER_STAGE_ANY_HIT_BIT_NV);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(ShaderStage::RAY_CLOSEST_HIT,	VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(ShaderStage::RAY_GENERATION,	VK_SHADER_STAGE_RAYGEN_BIT_NV);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(ShaderStage::RAY_INTERSECTION,	VK_SHADER_STAGE_INTERSECTION_BIT_NV);
		if (RenderingSystem::Instance->IsRayTracingSupported()) MAPPING(ShaderStage::RAY_MISS,			VK_SHADER_STAGE_MISS_BIT_NV);
		MAPPING(ShaderStage::TESSELLATION_CONTROL,	VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
		MAPPING(ShaderStage::TESSELLATION_CONTROL,	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
		MAPPING(ShaderStage::VERTEX,				VK_SHADER_STAGE_VERTEX_BIT);

#undef MAPPING

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
			case TextureFilter::LINEAR: return VkFilter::VK_FILTER_LINEAR;
			case TextureFilter::NEAREST: return VkFilter::VK_FILTER_NEAREST;

			default:
			{
#if defined(CATALYST_CONFIGURATION_DEBUG)
				ASSERT(false, "Unknown texture filter.");
#endif
				return VkFilter::VK_FILTER_NEAREST;
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
#endif