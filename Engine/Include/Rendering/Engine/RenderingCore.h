#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_TERRAIN_HEIGHT_TEXTURES{ 255 };
}

/*
*	Type aliases.
*/
using OpaqueHandle = void *RESTRICT;

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
	DirectionalShadow,
	Scene,
	WorldPosition,
	ParticleSystem,
#if defined(CATALYST_ENABLE_OCEAN)
	Ocean,
#endif
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	VolumetricFog,
#endif
#if !defined(CATALYST_FINAL)
	Debug,
#endif
	BloomHorizontal,
	BloomVertical,
	DepthOfFieldHorizontal,
	DepthOfFieldVertical,
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
	//Directional shadow main stage.
	DirectionalTerrainShadow,
	DirectionalDynamicPhysicalShadow,
	DirectionalSolidVegetationShadow,

	//Scene main stage.
	TerrainDepth,
	TerrainColor,
	DynamicPhysical,
	HighDetailSolidVegetation,
	MediumDetailSolidVegetation,
	LowDetailSolidVegetation,
	HighDetailGrassVegetationDepth,
	HighDetailGrassVegetationColor,
	MediumDetailGrassVegetationDepth,
	MediumDetailGrassVegetationColor,
	LowDetailGrassVegetationDepth,
	LowDetailGrassVegetationColor,
	DirectionalShadow,
	Lighting,
	Sky,
	DynamicOutline,

	//World position main stage.
	WorldPosition,

	//Particle system main stage.
	ParticleSystem,

#if defined(CATALYST_ENABLE_OCEAN)
	//Ocean main stage.
	AboveOcean,
	BelowOcean,
#endif

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	//Volumetric fog main stage.
	VolumetricFog,
#endif

#if !defined(CATALYST_FINAL)
	//Debug.
	DebugAxisAlignedBoundingBox,
	DebugScreenBox,
#endif

	//Bloom horizontal main stage.
	BloomHorizontal,

	//Bloom vertical main stage.
	BloomVertical,

	//Depth of field horizontal main stage.
	DepthOfFieldHorizontal,

	//Depth of field vertical main stage.
	DepthOfFieldVertical,

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
	TerrainMaterial,
	PhysicalMaterial,
#if defined(CATALYST_ENABLE_OCEAN)
	OceanMaterial,
#endif
	ParticleSystem,
	GaussianBlur,

	Terrain,

	NumberOfCommonRenderDataTableLayouts
};

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	//Directional Shadows.
	DirectionalShadowMap,
	DirectionalShadow,

	//Scene.
	SceneIntermediate,
	SceneBufferAlbedo,
	SceneBufferNormalDepth,
	SceneBufferMaterialProperties,
	Scene,

	//Screen.
	Screen,

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
#if defined(CATALYST_ENABLE_OCEAN)
	AboveOceanFragment,
	BelowOceanFragment,
#endif
	BloomHorizontalFragment,
	BloomVerticalFragment,
	BoxBlurFragment,
#if !defined(CATALYST_FINAL)
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
	GaussianBlurFragment,
	HighDetailGrassVegetationColorFragment,
	HighDetailGrassVegetationColorVertex,
	HighDetailGrassVegetationDepthFragment,
	HighDetailGrassVegetationDepthVertex,
	HighDetailSolidVegetationFragment,
	LightingFragment,
	LowDetailGrassVegetationColorFragment,
	LowDetailGrassVegetationColorVertex,
	LowDetailGrassVegetationDepthFragment,
	LowDetailGrassVegetationDepthVertex,
	LowDetailSolidVegetationFragment,
	MediumDetailGrassVegetationColorFragment,
	MediumDetailGrassVegetationColorVertex,
	MediumDetailGrassVegetationDepthFragment,
	MediumDetailGrassVegetationDepthVertex,
	MediumDetailSolidVegetationFragment,
	OutlineFragment,
	ParticleSystemFragment,
	ParticleSystemGeometry,
	ParticleSystemVertex,
	AntiAliasingFragment,
	PhysicalFragment,
	PhysicalVertex,
	ScreenSpaceAmbientOcclusionFragment,
	ShadowMapFragment,
	SkyFragment,
	SolidVegetationVertex,
	TerrainColorFragment,
	TerrainDepthFragment,
	TerrainVertex,
	ToneMappingFragment,
	ViewportVertex,
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	VolumetricFogFragment,
#endif
	WorldPositionFragment,

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

//Enumeration covering all stencil operators.
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
	Viewer = BIT(1),
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
	Vector2 direction;

	//The inverse resolution.
	Vector2 inverseResolution;
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