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

//Math.
#include <Math/General/Vector.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_TERRAIN_PATCHES{ UINT8_MAXIMUM };
	constexpr uint32 MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES{ 32 };

	constexpr uint32 SCENE_BUFFER_STENCIL_BIT{ BIT(0) };
	constexpr uint32 SCENE_BUFFER_GENERAL_STENCIL_BIT{ BIT(1) };
	constexpr uint32 SCENE_BUFFER_PARTICLE_SYSTEMS_STENCIL_BIT{ BIT(2) };
}

/*
*	Type aliases.
*/
using OpaqueHandle = void *RESTRICT; //Opaque handle that represents abstract data.

using AccelerationStructureHandle = void *RESTRICT;
using BufferHandle = void *RESTRICT;
using DepthBufferHandle = void *RESTRICT;
using RenderDataTableHandle = void *RESTRICT;
using RenderDataTableLayoutHandle = void *RESTRICT;
using RenderTargetHandle = void *RESTRICT;
using SamplerHandle = void *RESTRICT;
using Texture2DHandle = void *RESTRICT;
using TextureCubeHandle = void *RESTRICT;

/*
*	Definition of an empty handle.
*/
constexpr OpaqueHandle EMPTY_HANDLE{ nullptr };

/*
*	Enumeration covering all render pass stages.
*	The order of the stages define in what order they will execute in.
*/
enum class RenderPassStage : uint8
{
	WorldRayTracing,
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
	RenderOverride,
#endif
	IndirectLightingDenoising,
	DirectLightingDenoising,
	Lighting,
	TemporalAccumulation,
	RadianceIntegration,
	MotionBlur,
	PostProcessing,
	ToneMapping,
	AntiAliasing,

	NumberOfRenderPassStages
};

//Enumeration covering all physical flags.
enum class PhysicalFlag : uint8
{
	Physical = BIT(0),
	Outline = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(PhysicalFlag);

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

//Enumeration covering all common materials.
enum class CommonMaterial : uint8
{
	Gray,

	NumberOfCommonMaterials
};

//Enumeration covering all common render data table layouts.
enum class CommonRenderDataTableLayout : uint8
{
	Global,

	NumberOfCommonRenderDataTableLayouts
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

//Enumeration covering all level of details.
enum class LevelOfDetail : uint8
{
	Low,
	Medium,
	High,

	NumberOfLevelOfDetails
};

//Enumeration covering all memory properties.
enum class MemoryProperty : uint8
{
	DeviceLocal = BIT(0),
	HostCoherent = BIT(1),
	HostVisible = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(MemoryProperty);

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	/*
	*	Contains the indirect lighting.
	*/
	IndirectLighting,

	/*
	*	Intermediate render target, useful when pipelines needs to do ping-pong rendering.
	*/
	Intermediate,

	/*
	*	Contains an accumulated buffer of all previous color values in the scene.
	*/
	PreviousRadiance,

	/*
	*	Should contain the final color for the scene.
	*	Also used for intermediate purposes.
	*/
	Scene,

	/*
	*	Contains the albedo.
	*/
	SceneFeatures1,

	/*
	*	Contains the world space normal as well as the hit distance.
	*/
	SceneFeatures2,

	/*
	*	Contains the roughness, metallic and ambient occlusion.
	*/
	SceneFeatures3,

	/*
	*	Contains the hit instance ID and primitive ID.
	*/
	SceneFeatures4,

	/*
	*	For rendering directly to the screen.
	*/
	Screen,

	/*
	*	The first temporal accumulation buffer - The temporal accumulation render pass will ping-pong the first and second buffers.
	*/
	TemporalAccumulationBuffer1,

	/*
	*	The second temporal accumulation buffer - The temporal accumulation render pass will ping-pong the first and second buffers.
	*/
	TemporalAccumulationBuffer2,

	NumberOfRenderTargets
};

//Enumeration covering all samplers.
enum class Sampler : uint8
{
	FilterLinear_MipmapModeLinear_AddressModeClampToEdge,
	FilterLinear_MipmapModeLinear_AddressModeRepeat,
	FilterLinear_MipmapModeNearest_AddressModeClampToEdge,
	FilterLinear_MipmapModeNearest_AddressModeRepeat,
	FilterNearest_MipmapModeNearest_AddressModeClampToBorder,
	FilterNearest_MipmapModeNearest_AddressModeClampToEdge,

	NumberOfSamplers
};

//Enumeration covering all shaders.
enum class Shader : uint8
{
	AntiAliasingFragment,
	DebugAxisAlignedBoundingBoxFragment,
	DebugAxisAlignedBoundingBoxVertex,
	DebugScreenBoxFragment,
	DebugScreenBoxVertex,
	IndirectLightingDenoisingFragment,
	LightingCompute,
	MotionBlurFragment,
	PassthroughFragment,
	PostProcessingFragment,
	RadianceIntegrationFragment,
	TemporalAccumulationFragment,
	ToneMappingFragment,
	ViewportVertex,
	VisibilityDenoisingFragment,
	VisibilityRayMissShader,
	WorldRayClosestHitShader,
	WorldRayGenerationShader,
	WorldRayMissShader,

	NumberOfShaders,

	None
};

//Enumeration covering all shader stages.
enum class ShaderStage : uint16
{
	Compute					= BIT(0),
	Fragment				= BIT(1),
	Geometry				= BIT(2),
	RayAnyHit				= BIT(3),
	RayClosestHit			= BIT(4),
	RayGeneration			= BIT(5),
	RayIntersection			= BIT(6),
	RayMiss					= BIT(7),
	TessellationControl		= BIT(8),
	TessellationEvaluation	= BIT(9),
	Vertex					= BIT(10)
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

//Enumeration covering all visibility flags.
enum class VisibilityFlag : uint8
{
	None = BIT(0),
	Perceiver = BIT(1),
	DirectionalLight = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(VisibilityFlag);

//Enumeration covering all address modes.
enum class AddressMode : uint8
{
	ClampToBorder,
	ClampToEdge,
	MirrorClampToEdge,
	MirroredRepeat,
	Repeat
};

//Enumeration covering all mipmap modes.
enum class MipmapMode : uint8
{
	Linear,
	Nearest
};

//Enumeration covering all texture filters.
enum class TextureFilter : uint8
{
	Linear,
	Nearest
};

//Enumeration covering all texture formats.
enum class TextureFormat : uint8
{
	R8_Byte,
	R8G8B8A8_Byte,
	R32_Float,
	R32G32B32A32_Float
};

/*
*	Gaussian blur data.
*/
class GaussianBlurData final
{

public:

	//The direction.
	Vector2<float> _Direction;

	//The inverse resolution.
	Vector2<float> _InverseResolution;
};

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
	*	Constructor taking all values as arguments.
	*/
	constexpr RenderDataTableLayoutBinding(const uint32 initialBinding, const Type initialType, const uint32 initialNumberOfArrayElements, const ShaderStage initialShaderStage) NOEXCEPT
		:
		_Binding(initialBinding),
		_Type(initialType),
		_NumberOfArrayElements(initialNumberOfArrayElements),
		_ShaderStage(initialShaderStage)
	{

	}

};

/*
*	Render data table update information.
*/
class RenderDataTableUpdateInformation final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		Texture2D
	};

	//The binding.
	uint32 _Binding;

	//The type.
	Type _Type;

	//The handle.
	OpaqueHandle _Handle;

	/*
	*	Constructor taking all values as arguments.
	*/
	RenderDataTableUpdateInformation(const uint32 initialBinding, const Type initialType, OpaqueHandle initialHandle) NOEXCEPT
		:
		_Binding(initialBinding),
		_Type(initialType),
		_Handle(initialHandle)
	{

	}
};

/*
*	Sampler properties.
*/
class SamplerProperties final
{

public:

	//The magnification filter.
	TextureFilter _MagnificationFilter;

	//The mipmap mode.
	MipmapMode _MipmapMode;

	//The address mode.
	AddressMode _AddressMode;

	/*
	*	Constructor taking all values as arguments.
	*/
	SamplerProperties(const TextureFilter initialMagnificationFilter, const MipmapMode initialMipmapMode, const AddressMode initialAddressMode) NOEXCEPT
		:
		_MagnificationFilter(initialMagnificationFilter),
		_MipmapMode(initialMipmapMode),
		_AddressMode(initialAddressMode)
	{

	}

	/*
	*	Equality operator overload.
	*/
	bool operator==(const SamplerProperties &other) const NOEXCEPT
	{
		return	_MagnificationFilter == other._MagnificationFilter
				&& _MipmapMode == other._MipmapMode
				&& _AddressMode == other._AddressMode;
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
		X32SignedInt
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