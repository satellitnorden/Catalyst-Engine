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
#include <Math/Vector2.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_TERRAIN_PATCHES{ UINT8_MAXIMUM };
	constexpr uint32 MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES{ 512 };

	constexpr uint32 SCENE_BUFFER_STENCIL_BIT{ BIT(0) };
	constexpr uint32 SCENE_BUFFER_GENERAL_STENCIL_BIT{ BIT(1) };
	constexpr uint32 SCENE_BUFFER_PARTICLE_SYSTEMS_STENCIL_BIT{ BIT(2) };
}

/*
*	Type aliases.
*/
using OpaqueHandle = void *RESTRICT; //Opaque handle that represents abstract data.

using ConstantBufferHandle = void *RESTRICT;
using DepthBufferHandle = void *RESTRICT;
using RenderDataTableHandle = void *RESTRICT;
using RenderDataTableLayoutHandle = void *RESTRICT;
using RenderTargetHandle = void *RESTRICT;
using SamplerHandle = void *RESTRICT;
using Texture2DHandle = void *RESTRICT;
using TextureCubeHandle = void *RESTRICT;
using UniformBufferHandle = void *RESTRICT;

/*
*	Enumeration covering all render pass main stages.
*/
enum class RenderPassMainStage : uint8
{
	None,
	DirectionalShadowMapping,
	Scene,
	DirectionalShadowCalculation,
	DirectionalShadowHorizontalBlur,
	DirectionalShadowVerticalBlur,
	ScreenSpaceAmbientOcclusionCalculation,
	ScreenSpaceAmbientOcclusionHorizontalBlur,
	ScreenSpaceAmbientOcclusionVerticalBlur,
	Lighting,
	Ocean,
	ExponentialHeightFog,
	DepthOfFieldHorizontal,
	DepthOfFieldVertical,
	VolumetricFog,
#if defined(CATALYST_CONFIGURATION_DEBUG)
	Debug,
#endif
	BloomDownsampleFirstIteration,
	BloomDownsampleSecondIteration,
	BloomDownsampleThirdIteration,
	BloomUpsampleFirstIteration,
	BloomUpsampleSecondIteration,
	BloomUpsampleThirdIteration,
	ToneMapping,
	AntiAliasing,

	NumberOfRenderPassMainStages
};

/*
*	Enumeration covering all render pass sub stages.
*	The order of the render pass sub stage defines the order that the render passes will be rendered in.
*/
enum class RenderPassSubStage : uint8
{
	//Directional shadow mapping main stage.
	DirectionalTerrainShadow,
	DirectionalDynamicPhysicalShadow,
	DirectionalTreeVegetationTrunkShadow,
	DirectionalTreeVegetationCrownShadow,
	DirectionalSolidVegetationShadow,

	//Scene main stage.
	TerrainDepth,
	TerrainColor,
	DynamicPhysical,
	HighDetailTreeVegetationTrunk,
	MediumDetailTreeVegetationTrunk,
	LowDetailTreeVegetationTrunk,
	HighDetailTreeVegetationCrownDepth,
	HighDetailTreeVegetationCrownColor,
	MediumDetailTreeVegetationCrownDepth,
	MediumDetailTreeVegetationCrownColor,
	LowDetailTreeVegetationCrownDepth,
	LowDetailTreeVegetationCrownColor,
	HighDetailTreeVegetationImpostorDepth,
	HighDetailTreeVegetationImpostorColor,
	LowDetailTreeVegetationImpostorDepth,
	LowDetailTreeVegetationImpostorColor,
	HighDetailSolidVegetation,
	MediumDetailSolidVegetation,
	LowDetailSolidVegetation,
	HighDetailDebrisVegetation,
	MediumDetailDebrisVegetation,
	LowDetailDebrisVegetation,
	HighDetailGrassVegetationDepth,
	HighDetailGrassVegetationColor,
	MediumDetailGrassVegetationDepth,
	MediumDetailGrassVegetationColor,
	LowDetailGrassVegetationDepth,
	LowDetailGrassVegetationColor,
	ParticleSystem,

	//Directional shadow calculation main stage.
	DirectionalShadow,

	//Directional shadow horizontal blur main stage.
	DirectionalShadowHorizontalBlur,

	//Directional shadow horizontal blur main stage.
	DirectionalShadowVerticalBlur,

	//Screen space ambient occlusion calculation main stage.
	ScreenSpaceAmbientOcclusion,

	//Screen space ambient occlusion horizontal blur main stage.
	ScreenSpaceAmbientOcclusionHorizontalBlur,

	//Screen space ambient occlusion vertical blur main stage.
	ScreenSpaceAmbientOcclusionVerticalBlur,

	//Lighting main stage.
	Lighting,
	PointLight,
	Sky,
	DynamicOutline,

	//Ocean main stage.
	AboveOcean,
	BelowOcean,

	//Exponential height fog main stage.
	ExponentialHeightFog,

	//Depth of field horizontal main stage.
	DepthOfFieldHorizontal,

	//Depth of field vertical main stage.
	DepthOfFieldVertical,

	//Volumetric fog main stage.
	VolumetricFog,

#if defined(CATALYST_CONFIGURATION_DEBUG)
	//Debug.
	DebugAxisAlignedBoundingBox,
	DebugScreenBox,
#endif

	//Bloom downsample first iteration main stage.
	BloomDownsampleFirstIteration,

	//Bloom downsample second iteration main stage.
	BloomDownsampleSecondIteration,

	//Bloom downsample third iteration main stage.
	BloomDownsampleThirdIteration,

	//Bloom upsample first iteration main stage.
	BloomUpsampleFirstIteration,

	//Bloom upsample second iteration main stage.
	BloomUpsampleSecondIteration,

	//Bloom upsample third iteration main stage.
	BloomUpsampleThirdIteration,

	//Tone mapping main stage.
	ToneMapping,

	//Anti-aliasing main stage.
	AntiAliasing,

	NumberOfRenderPassSubStages
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
	UniformBuffer = BIT(0),
	VertexBuffer = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(BufferUsage);

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

//Enumeration covering all depth buffers.
enum class DepthBuffer : uint8
{
	DirectionalLight,
	SceneBuffer,

	NumberOfDepthBuffers,

	None
};

//Enumeration covering all common render data table layouts.
enum class CommonRenderDataTableLayout : uint8
{
	Global,
	EnvironmentMaterial,
	GrassMaterial,
	OceanMaterial,
	ParticleSystem,
	GaussianBlur,

	NumberOfCommonRenderDataTableLayouts
};

//Enumeration covering all level of details.
enum class LevelOfDetail : uint8
{
	Low,
	Medium,
	High,

	NumberOfLevelOfDetails
};

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	//Directional Shadows.
	DirectionalShadowMap,
	DirectionalShadow,

	//Intermediate.
	Intermediate,
	IntermediateHalf,
	IntermediateQuarter,
	IntermediateEighth,

	//Scene.
	SceneBufferAlbedo,
	SceneBufferNormalDepth,
	SceneBufferMaterialProperties,
	Scene,
	SceneProperties,

	//Screen.
	Screen,

	//Screen space ambient occlusion.
	ScreenSpaceAmbientOcclusion,

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
	AboveOceanFragment,
	BelowOceanFragment,
	BloomDownsampleFirstIterationFragment,
	BloomDownsampleFragment,
	BloomUpsampleFragment,
	BoxBlurFragment,
#if defined(CATALYST_CONFIGURATION_DEBUG)
	DebugAxisAlignedBoundingBoxFragment,
	DebugAxisAlignedBoundingBoxVertex,
	DebugScreenBoxFragment,
	DebugScreenBoxVertex,
#endif
	DepthOfFieldFragment,
	DirectionalPhysicalShadowVertex,
	DirectionalShadowFragment,
	DirectionalSolidVegetationShadowFragment,
	DirectionalSolidVegetationShadowVertex,
	DirectionalTerrainShadowVertex,
	DirectionalTreeVegetationCrownShadowFragment,
	DirectionalTreeVegetationCrownShadowVertex,
	DirectionalTreeVegetationTrunkVertex,
	ExponentialHeightFogFragment,
	GaussianBlurFragment,
	HighDetailDebrisVegetationFragment,
	HighDetailDebrisVegetationVertex,
	HighDetailGrassVegetationColorFragment,
	HighDetailGrassVegetationColorVertex,
	HighDetailGrassVegetationDepthFragment,
	HighDetailGrassVegetationDepthVertex,
	HighDetailSolidVegetationFragment,
	HighDetailSolidVegetationVertex,
	HighDetailTreeVegetationCrownColorFragment,
	HighDetailTreeVegetationCrownColorVertex,
	HighDetailTreeVegetationCrownDepthFragment,
	HighDetailTreeVegetationCrownDepthVertex,
	HighDetailTreeVegetationImpostorColorFragment,
	HighDetailTreeVegetationImpostorColorGeometry,
	HighDetailTreeVegetationImpostorColorVertex,
	HighDetailTreeVegetationImpostorDepthFragment,
	HighDetailTreeVegetationImpostorDepthGeometry,
	HighDetailTreeVegetationImpostorDepthVertex,
	HighDetailTreeVegetationTrunkFragment,
	HighDetailTreeVegetationTrunkVertex,
	LightingFragment,
	LowDetailDebrisVegetationFragment,
	LowDetailDebrisVegetationVertex,
	LowDetailGrassVegetationColorFragment,
	LowDetailGrassVegetationColorVertex,
	LowDetailGrassVegetationDepthFragment,
	LowDetailGrassVegetationDepthVertex,
	LowDetailSolidVegetationFragment,
	LowDetailSolidVegetationVertex,
	LowDetailTreeVegetationCrownColorFragment,
	LowDetailTreeVegetationCrownColorVertex,
	LowDetailTreeVegetationCrownDepthFragment,
	LowDetailTreeVegetationCrownDepthVertex,
	LowDetailTreeVegetationImpostorColorFragment,
	LowDetailTreeVegetationImpostorColorGeometry,
	LowDetailTreeVegetationImpostorColorVertex,
	LowDetailTreeVegetationImpostorDepthFragment,
	LowDetailTreeVegetationImpostorDepthGeometry,
	LowDetailTreeVegetationImpostorDepthVertex,
	LowDetailTreeVegetationTrunkFragment,
	LowDetailTreeVegetationTrunkVertex,
	MediumDetailDebrisVegetationFragment,
	MediumDetailDebrisVegetationVertex,
	MediumDetailGrassVegetationColorFragment,
	MediumDetailGrassVegetationColorVertex,
	MediumDetailGrassVegetationDepthFragment,
	MediumDetailGrassVegetationDepthVertex,
	MediumDetailSolidVegetationFragment,
	MediumDetailSolidVegetationVertex,
	MediumDetailTreeVegetationCrownColorFragment,
	MediumDetailTreeVegetationCrownColorVertex,
	MediumDetailTreeVegetationCrownDepthFragment,
	MediumDetailTreeVegetationCrownDepthVertex,
	MediumDetailTreeVegetationTrunkFragment,
	MediumDetailTreeVegetationTrunkVertex,
	OutlineFragment,
	ParticleSystemFragment,
	ParticleSystemGeometry,
	ParticleSystemVertex,
	AntiAliasingFragment,
	PhysicalFragment,
	PhysicalVertex,
	PointLightFragment,
	PointLightVertex,
	ScreenSpaceAmbientOcclusionBlurFragment,
	ScreenSpaceAmbientOcclusionFragment,
	ShadowMapFragment,
	SkyFragment,
	TerrainColorFragment,
	TerrainDepthFragment,
	TerrainVertex,
	ToneMappingFragment,
	ViewportVertex,
	VolumetricFogFragment,

	NumberOfShaders,

	None
};

//Enumeration covering all shader stages.
enum class ShaderStage : uint8
{
	Vertex = BIT(0),
	TessellationControl = BIT(1),
	TessellationEvaluation = BIT(2),
	Geometry = BIT(3),
	Fragment = BIT(4),
	Compute = BIT(5)
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
		CombinedImageSampler,
		SampledImage,
		Sampler,
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