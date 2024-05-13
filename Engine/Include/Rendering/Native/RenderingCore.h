/*
*	Explanation of stencil operations.
*
*	The stencil fail operator is the action performed on samples that fail the stencil test.
*	The stencil pass operator is the action performed on samples that pass both the depth and stencil tests.
*	The stencil depth fail operator is the action performed on samples that pass the stencil test and fail the depth test.
*	The compare operator is the comparison operator used in the stencil test.
*	The compare mask selects the bits of the unsigned integer stencil values participating in the stencil test.
*	The write mask selects the bits of the unsigned integer stencil values updated by the stencil test in the stencil framebuffer attachment.
*	The reference mask is an integer reference value that is used in the unsigned stencil comparison.
*/

#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#define CATALYST_SHADER_LANGUAGE_CXX
#include <Rendering/Native/Shader/CatalystShaderConstants.h>

//Forward declarations.
class RenderPass;

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_MESHES_PER_MODEL{ 8 };

	constexpr uint32 SCENE_BUFFER_STENCIL_BIT{ BIT(0) };

	constexpr uint32 TERRAIN_HIT_GROUP_INDEX{ 0 };
	constexpr uint32 STATIC_MODELS_HIT_GROUP_INDEX{ 1 };
	constexpr uint32 DYNAMIC_MODELS_HIT_GROUP_INDEX{ 2 };
}

/*
*	Type aliases.
*/
using OpaqueHandle = void *RESTRICT; //Opaque handle that represents abstract data.

using AccelerationStructureHandle = void *RESTRICT;
using BufferHandle = void *RESTRICT;
using CommandPoolHandle = void *RESTRICT;
using DepthBufferHandle = void *RESTRICT;
using EventHandle = void *RESTRICT;
using QueryPoolHandle = void *RESTRICT;
using RenderDataTableHandle = void *RESTRICT;
using RenderDataTableLayoutHandle = void *RESTRICT;
using RenderTargetHandle = void *RESTRICT;
using SamplerHandle = void *RESTRICT;
using ShaderHandle = void *RESTRICT;
using Texture2DHandle = void* RESTRICT;
using Texture3DHandle = void* RESTRICT;
using TextureCubeHandle = void *RESTRICT;

using CustomRenderingPathCallback = void(*)(DynamicArray<RenderPass *RESTRICT> *const RESTRICT render_passes);

/*
*	Definition of an empty handle.
*/
constexpr OpaqueHandle EMPTY_HANDLE{ nullptr };

/*
*	Enumeration covering all rendering platform events.
*/
enum class RenderingPlatformEvent : uint8
{
	SURFACE_GAINED = BIT(0),
	SURFACE_LOST = BIT(1)
};

/*
*	Enumeration covering all sub rendering systems.
*/
enum class SubRenderingSystem : uint8
{
	OPEN_GL,
	VULKAN
};

//Enumeration covering all rendering paths.
enum class RenderingPath : uint8
{
	NONE,

	DEFAULT,
	MOBILE,
	VIRTUAL_REALITY,

	CUSTOM,

	NUMBER_OF_RENDERING_PATHS
};

/*
*	Enumeration covering all default native render pass stages.
*	The order of the stages define in what order they will execute in.
*/
enum class DefaultNativeRenderPassStage : uint8
{
	SCENE_FEATURES,
	POST_SCENE_FEATURES,
	SHADOWS,
	IRRADIANCE,
	AMBIENT_OCCLUSION,
	DIRECT_LIGHTING,
	INDIRECT_LIGHTING,
	SKY,
	VOLUMETRIC_LIGHTING,
	WATER,
#if !defined(CATALYST_CONFIGURATION_FINAL)
	DEBUG_RENDERING,
#endif
	BLOOM,
	TONE_MAPPING,
	ANTI_ALIASING,
	DEPTH_OF_FIELD,
	POST_PROCESSING,
#if !defined(CATALYST_CONFIGURATION_FINAL)
	DEBUG,
#endif
	USER_INTERFACE,
#if !defined(CATALYST_CONFIGURATION_FINAL)
	IMGUI,
#endif
	SCREENSHOT_ACCUMULATION,
	SCREEN,

	NUMBER_OF_STAGES
};

/*
*	Enumeration covering all mobile native render pass stages.
*	The order of the stages define in what order they will execute in.
*/
enum class MobileNativeRenderPassStage : uint8
{
	MOBILE,
#if !defined(CATALYST_CONFIGURATION_FINAL)
	IMGUI,
#endif

	NUMBER_OF_STAGES
};

/*
*	Enumeration covering all virtual reality native render pass stages.
*	The order of the stages define in what order they will execute in.
*/
enum class VirtualRealityNativeRenderPassStage : uint8
{
	VIRTUAL_REALITY,
#if defined(CATALYST_PLATFORM_WINDOWS)
	VIRTUAL_REALITY_VISUALIZATION,
#endif
#if !defined(CATALYST_CONFIGURATION_FINAL)
	IMGUI,
#endif

	NUMBER_OF_STAGES
};

//Enumeration covering all access flags.
enum class AccessFlags : uint8
{
	NONE = 0,
	SHADER_READ = BIT(0),
	SHADER_WRITE = BIT(1),
	COLOR_ATTACHMENT_READ = BIT(2),
	COLOR_ATTACHMENT_WRITE = BIT(3),
	MEMORY_READ = BIT(4),
	MEMORY_WRITE = BIT(5)
};

ENUMERATION_BIT_OPERATIONS(AccessFlags);

//Enumeration covering all attachment load operators.
enum class AttachmentLoadOperator : uint8
{
	LOAD,
	CLEAR,
	DONT_CARE
};

//Enumeration covering all attachment store operators.
enum class AttachmentStoreOperator : uint8
{
	STORE,
	DONT_CARE
};

//Enumeration covering all blend factors.
enum class BlendFactor : uint8
{
	Zero,
	One,
	SourceColor,
	OneMinusSourceColor,
	DestinationColor,
	OneMinusDestinationColor,
	SourceAlpha,
	OneMinusSourceAlpha,
	DestinationAlpha,
	OneMinusDestinationAlpha,
};

//Enumeration covering all blender operators.
enum class BlendOperator : uint8
{
	ADD,
	SUBTRACT,
	REVERSE_SUBTRACT,
	MIN,
	MAX
};

//Enumeration covering all bottom level acceleration structure flags.
enum class BottomLevelAccelerationStructureFlag : uint8
{
	NONE = 0,
	OPAQUE = BIT(0)
};

ENUMERATION_BIT_OPERATIONS(BottomLevelAccelerationStructureFlag);

//Enumeration covering all buffer usages.
enum class BufferUsage : uint8
{
	IndexBuffer = BIT(0),
	StorageBuffer = BIT(1),
	RayTracing = BIT(2),
	UniformBuffer = BIT(3),
	VertexBuffer = BIT(4)
};

ENUMERATION_BIT_OPERATIONS(BufferUsage);

//Enumeration covering all command buff levels.
enum class CommandBufferLevel : uint8
{
	PRIMARY,
	SECONDARY
};

//Enumeration covering all common render data table layouts.
enum class CommonRenderDataTableLayout : uint8
{
	CLOUDS,
	GLOBAL,
	GLOBAL_2,
	PARTICLE_SYSTEM,
	USER_INTERFACE,

	NUMBER_OF_COMMON_RENDER_DATA_TABLE_LAYOUTS
};

//Enumeration covering all common render data tables.
enum class CommonRenderDataTable : uint8
{
	CLOUDS,

	NUMBER_OF_COMMON_RENDER_DATA_TABLES
};

//Enumeration covering all compare operators.
enum class CompareOperator : uint8
{
	Always,
	Equal,
	Greater,
	GreaterOrEqual,
	Less,
	LessOrEqual,
	Never,
	NotEqual
};

//Enumeration covering all cull modes.
enum class CullMode : uint8
{
	None,
	Back,
	Front,
	FrontAndBack
};

//Enumeration covering all image layouts.
enum class ImageLayout : uint8
{
	UNDEFINED,
	GENERAL,
	COLOR_ATTACHMENT_OPTIMAL,
	DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	DEPTH_STENCIL_READ_ONLY_OPTIMAL,
	SHADER_READ_ONLY_OPTIMAL
};

//Enumeration covering all memory properties.
enum class MemoryProperty : uint8
{
	DeviceLocal = BIT(0),
	HostCoherent = BIT(1),
	HostVisible = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(MemoryProperty);

//Enumeration covering all models flags.
enum class ModelFlags : uint32
{
	NONE = 0,
	INCLUDE_IN_SHADOW_CASCADE_1 = BIT(0),
	INCLUDE_IN_SHADOW_CASCADE_2 = BIT(1),
	INCLUDE_IN_SHADOW_CASCADE_3 = BIT(2),
	INCLUDE_IN_SHADOW_CASCADE_4 = BIT(3),
	IS_VEGETATION = BIT(4),
	SNAP_TO_TERRAIN = BIT(5)
};

ENUMERATION_BIT_OPERATIONS(ModelFlags);

//Enumeration covering all pipeline stage flags.
enum class PipelineStageFlags : uint8
{
	NONE = 0,
	FRAGMENT_SHADER = BIT(0),
	COLOR_ATTACHMENT_OUTPUT = BIT(1),
	COMPUTE_SHADER = BIT(2),
	RAY_TRACING_SHADER = BIT(3),
};

ENUMERATION_BIT_OPERATIONS(PipelineStageFlags);

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	/*
	*	Intermediate render target at half resolution.
	*/
	INTERMEDIATE_RGBA_FLOAT32_HALF_2,

	NUMBER_OF_RENDER_TARGETS
};

//Enumeration covering all shared render targets.
enum class SharedRenderTarget : uint8
{
	/*
	*	Contains the albedo in the RGB channels and the material index in the A channel.
	*/
	SCENE_FEATURES_1,

	/*
	*	Contains the shading normal in the RGB channels and the depth in the A channel.
	*/
	SCENE_FEATURES_2,

	/*
	*	Contains the roughness in the R channel, metallic in the G channel, ambient occlusion in the B channel and the luminance in the A channel.
	*/
	SCENE_FEATURES_3,

	/*
	*	Contains the velocity in the RG channels.
	*/
	SCENE_FEATURES_4,

	/*
	*	Contains the albedo in the RGB channels and the material index in the A channel at half resolution.
	*/
	SCENE_FEATURES_1_HALF,

	/*
	*	Contains the shading normal in the RGB channels and the depth in the A channel at half resolution.
	*/
	SCENE_FEATURES_2_HALF,

	/*
	*	Contains the roughness in the R channel, metallic in the G channel, ambient occlusion in the B channel and the luminance in the A channel at half resolution.
	*/
	SCENE_FEATURES_3_HALF,

	/*
	*	Contains the velocity in the RG channels at half resolution.
	*/
	SCENE_FEATURES_4_HALF,

	/*
	*	Contains the diffuse irradiance.
	*/
	DIFFUSE_IRRADIANCE,

	/*
	*	Contains the specular irradiance.
	*/
	SPECULAR_IRRADIANCE,

	/*
	*	Contains the final color for the scene, in high dynamic range.
	*/
	SCENE,

	/*
	*	Contains the downsampled bloom at half resolution.
	*/
	BLOOM_HALF,

	/*
	*	Contains the downsampled bloom at quarter resolution.
	*/
	BLOOM_QUARTER,

	/*
	*	Contains the downsampled bloom at eighth resolution.
	*/
	BLOOM_EIGHTH,

	/*
	*	Contains the downsampled bloom at sixteenth resolution.
	*/
	BLOOM_SIXTEENTH,

	/*
	*	Contains the downsampled bloom at thirtysecond resolution.
	*/
	BLOOM_THIRTYSECOND,

	/*
	*	Contains the final color for the previous scene, in high dynamic range.
	*/
	PREVIOUS_SCENE,

	/*
	*	Contains the final color for the scene, in low dynamic range.
	*/
	SCENE_LOW_DYNAMIC_RANGE_1,

	/*
	*	Contains the final color for the scene, in low dynamic range.
	*/
	SCENE_LOW_DYNAMIC_RANGE_2,

	/*
	*	Intermediate render target.
	*/
	INTERMEDIATE_RGBA_FLOAT32_1,

	/*
	*	Intermediate render target.
	*/
	INTERMEDIATE_RGBA_FLOAT32_2,

	/*
	*	Intermediate render target at half resolution.
	*/
	INTERMEDIATE_RGBA_FLOAT32_HALF_1,

	/*
	*	Intermediate render target at quarter resolution.
	*/
	INTERMEDIATE_RGBA_FLOAT32_QUARTER_1,

	/*
	*	Intermediate render target at quarter resolution.
	*/
	INTERMEDIATE_RGBA_FLOAT32_QUARTER_2,

	/*
	*	The scene depth buffer.
	*/
	SCENE_DEPTH_BUFFER,

#if defined(CATALYST_EDITOR)
	/*
	*	The editor viewport.	
	*/
	EDITOR_VIEWPORT,
#endif

	NUMBER_OF_SHARED_RENDER_TARGETS
};

//Enumeration covering all sample counts.
enum class SampleCount : uint8
{
	SAMPLE_COUNT_1,
	SAMPLE_COUNT_2,
	SAMPLE_COUNT_4,
	SAMPLE_COUNT_8,
	SAMPLE_COUNT_16,
	SAMPLE_COUNT_32,
	SAMPLE_COUNT_64
};

//Enumeration covering all samplers.
enum class Sampler : uint8
{
	FilterLinear_MipmapModeLinear_AddressModeClampToEdge,
	FilterLinear_MipmapModeLinear_AddressModeRepeat,
	FilterLinear_MipmapModeNearest_AddressModeClampToEdge,
	FilterLinear_MipmapModeNearest_AddressModeRepeat,
	FilterNearest_MipmapModeNearest_AddressModeClampToEdge,
	FilterNearest_MipmapModeNearest_AddressModeRepeat,

	NumberOfSamplers
};

//Enumeration covering all shader stages.
enum class ShaderStage : uint16
{
	COMPUTE					= BIT(0),
	FRAGMENT				= BIT(1),
	GEOMETRY				= BIT(2),
	RAY_ANY_HIT				= BIT(3),
	RAY_CLOSEST_HIT			= BIT(4),
	RAY_GENERATION			= BIT(5),
	RAY_INTERSECTION		= BIT(6),
	RAY_MISS				= BIT(7),
	TESSELLATION_CONTROL	= BIT(8),
	TESSELLATION_EVALUATION	= BIT(9),
	VERTEX					= BIT(10)
};

ENUMERATION_BIT_OPERATIONS(ShaderStage);

/*
*	Enumeration covering all stencil operators.
*
*	Keep keeps the current value.
*	Zero the the value to 0.
*	Replace sets the value to the reference mask.
*	IncrementAndClamp increments the current value and clamps to the maximum representable unsigned value.
*	DecrementAndClamp decrements the current value and clamps to 0.
*	Invert bitwise-inverts the current value.
*	IncrementAndWrap increments the current value and wraps to 0 when the maximum value would have been exceeded.
*	DecrementAndWrap decrements the current value and wraps to the maximum possible value when the value would go below 0.
*/
enum class StencilOperator : uint8
{
	Keep,
	Zero,
	Replace,
	IncrementAndClamp,
	DecrementAndClamp,
	Invert,
	IncrementAndWrap,
	DecrementAndWrap
};

//Enumeration covering all surface transforms.
enum class SurfaceTransform : uint8
{
	IDENTITY,
	ROTATE_90,
	ROTATE_180,
	ROTATE_270,
};

//Enumeration covering all topologies.
enum class Topology : uint8
{
	LineList,
	LineListWithAdjacency,
	LineStrip,
	LineStripWithAdjacency,
	PatchList,
	PointList,
	TriangleFan,
	TriangleList,
	TriangleListWithAdjacency,
	TriangleStrip,
	TriangleStripWithAdjacency
};

//Enumeration covering all address modes.
enum class AddressMode : uint8
{
	CLAMP_TO_BORDER,
	CLAMP_TO_EDGE,
	MIRROR_CLAMP_TO_EDGE,
	MIRRORED_REPEAT,
	REPEAT
};

//Enumeration covering all mipmap modes.
enum class MipmapMode : uint8
{
	LINEAR,
	NEAREST
};

//Enumeration covering all mipmap generation modes.
enum class MipmapGenerationMode : uint8
{
	/*
	*	Will not generate mipmaps for this mode.
	*/
	NONE,

	/*
	*	Will just do a blend of all 4 source texels.
	*/
	DEFAULT,

	/*
	*	Will take the maximum of the 4 source texels.
	*/
	MAXIMUM,

	/*
	*	Will do a blend of all 4 source texels, but normalize them afterwards.
	*/
	NORMAL_MAP,

	/*
	*	Will perform a special algorithm tailored for opacity textures.
	*/
	OPACITY
};

//Enumeration covering all texture filters.
enum class TextureFilter : uint8
{
	LINEAR,
	NEAREST
};

//Enumeration covering all texture formats.
enum class TextureFormat : uint8
{
	//General formats.
	R_UINT8,
	R_UINT16,
	RGB_UINT8,
	RGBA_UINT8,
	RGBA_BC7,
	RG_FLOAT16,
	R_FLOAT32,
	RG_FLOAT32,
	RGBA_FLOAT32,

	//Depth formats.
	D_UINT16,
	D_UINT24_S_UINT8,
};

//Enumeration covering all texture usages.
enum class TextureUsage : uint8
{
	NONE,
	STORAGE
};

//Enumeration covering all visibility flags.
enum class VisibilityFlags : uint8
{
	NONE = 0,
	CAMERA = BIT(0),
	SHADOW_MAP_START = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(VisibilityFlags);

/*
*	Push constant range.
*/
class PushConstantRange final
{

public:

	//The shader stage.
	ShaderStage shaderStage;

	//The offset.
	uint32 offset;

	//The size.
	uint32 size;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	PushConstantRange() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	PushConstantRange(const ShaderStage initialShaderStage, const uint32 initialOffset, const uint32 initialSize)
		:
		shaderStage(initialShaderStage),
		offset(initialOffset),
		size(initialSize)
	{

	}

};

/*
*	Render data table layout information.
*/
class RenderDataTableLayoutBinding final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		AccelerationStructure,
		CombinedImageSampler,
		SampledImage,
		Sampler,
		StorageBuffer,
		StorageImage,
		UniformBuffer
	};

	//The binding.
	uint32 _Binding;

	//The type.
	Type _Type;

	//The number of array elements.
	uint32 _NumberOfArrayElements;

	//The shader stage.
	ShaderStage _ShaderStage;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr RenderDataTableLayoutBinding() NOEXCEPT
		:
		_Binding(0),
		_Type(Type::AccelerationStructure),
		_NumberOfArrayElements(0),
		_ShaderStage(ShaderStage::COMPUTE)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr RenderDataTableLayoutBinding(const uint32 initialBinding, const Type initialType, const uint32 initialNumberOfArrayElements, const ShaderStage initialShaderStage) NOEXCEPT
		:
		_Binding(initialBinding),
		_Type(initialType),
		_NumberOfArrayElements(initialNumberOfArrayElements),
		_ShaderStage(initialShaderStage)
	{

	}

};

/*
*	Vertex input attribute description.
*/
class VertexInputAttributeDescription final
{

public:

	//Enum covering all formats.
	enum class Format : uint8
	{
		X32SignedFloat,
		X32Y32SignedFloat,
		X32Y32Z32SignedFloat,
		X32Y32Z32W32SignedFloat,
		X32SignedInt,
		X32UnsignedInt,
		X32Y32Z32SignedInt
	};

	//The location.
	uint32 _Location;

	//The binding.
	uint32 _Binding;

	//The format.
	Format _Format;

	//The offset.
	uint32 _Offset;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VertexInputAttributeDescription() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VertexInputAttributeDescription(const uint32 initialLocation, const uint32 initialBinding, const Format initialFormat, const uint32 initialOffset)
		:
		_Location(initialLocation),
		_Binding(initialBinding),
		_Format(initialFormat),
		_Offset(initialOffset)
	{

	}

};

/*
*	Vertex input binding description.
*/
class VertexInputBindingDescription final
{

public:

	//Enum covering all input rates.
	enum class InputRate : uint8
	{
		Instance,
		Vertex
	};

	//The binding.
	uint32 _Binding;

	//The stride.
	uint32 _Stride;

	//The input rate.
	InputRate _InputRate;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VertexInputBindingDescription() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VertexInputBindingDescription(const uint32 initialBinding, const uint32 initialStride, const InputRate initialInputRate)
		:
		_Binding(initialBinding),
		_Stride(initialStride),
		_InputRate(initialInputRate)
	{

	}

};