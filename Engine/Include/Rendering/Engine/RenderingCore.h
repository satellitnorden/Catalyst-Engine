#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint8 SCREEN_SPACE_AMBIENT_OCCLUSION_RANDOM_KERNEL_SIZE{ 16 };
	constexpr uint8 SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLE_KERNEL_SIZE{ 8 };
	constexpr float SHADOW_VIEW_DISTANCE{ 100.0f };
}

/*
*	Type aliases.
*/
using OpaqueHandle = void *RESTRICT;

using ConstantBufferHandle = void *RESTRICT;
using RenderDataTableHandle = void *RESTRICT;
using RenderDataTableLayoutHandle = void *RESTRICT;
using RenderTargetHandle = void *RESTRICT;
using Texture2DHandle = void *RESTRICT;
using TextureCubeMapHandle = void *RESTRICT;
using UniformBufferHandle = void *RESTRICT;

/*
*	Enumeration covering all render pass main stages.
*/
enum class RenderPassMainStage : uint8
{
	None, //This will be removed once all the main stages are in place.
	DirectionalShadow,
	Scene,

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
	DirectionalStaticPhysicalShadow,
	DirectionalInstancedPhysicalShadow,

	//Scene main stage.
	Terrain,
	StaticPhysical,
	InstancedPhysical,
	Vegetation,
	DirectionalShadow,
	Lighting,
	Sky,
	ParticleSystem,

	//Post processing final main stage.
	PostProcessing,

	NumberOfRenderPassSubStages
};

//Enumeration covering all attachment load operators.
enum class AttachmentLoadOperator : uint8
{
	Clear,
	DontCare,
	Load
};

//Enumeration covering all attachment store operators.
enum class AttachmentStoreOperator : uint8
{
	DontCare,
	Store
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
	DynamicUniformData,
	Environment,
	Terrain,
	Physical,
	Vegetation,
	Ocean,
	ParticleSystem,
	GaussianBlur,
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

//Enumeration covering all shaders.
enum class Shader : uint8
{
	BloomFragment,
	BoxBlurFragment,
	CubeMapVertex,
	DirectionalPhysicalShadowVertex,
	DirectionalShadowFragment,
	DirectionalInstancedPhysicalShadowVertex,
	DirectionalTerrainShadowTessellationEvaluation,
	GaussianBlurFragment,
	InstancedPhysicalVertex,
	LightingFragment,
	OceanFragment,
	ParticleSystemFragment,
	ParticleSystemGeometry,
	ParticleSystemVertex,
	PostProcessingBloomFragment,
	PostProcessingFragment,
	PhysicalFragment,
	PhysicalVertex,
	ScreenSpaceAmbientOcclusionFragment,
	ShadowMapFragment,
	SkyFragment,
	TerrainFragment,
	TerrainTessellationControl,
	TerrainTessellationEvaluation,
	TerrainVertex,
	VegetationFragment,
	VegetationGeometry,
	VegetationVertex,
	ViewportVertex,

	NumberOfShaders,

	None
};

//Enumeration covering all shader stages.
enum class ShaderStage : uint8
{
	Vertex,
	TessellationControl,
	TessellationEvaluation,
	Geometry,
	Fragment,
	Compute
};

//Enumeration covering all special textures.
enum class SpecialTexture : uint8
{
	ScreenSpaceAmbientOcclusionRandomNoise,

	NumberOfSpecialTextures
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

//Enumeration covering all uniform buffers.
enum class UniformBuffer : uint8
{
	ScreenSpaceAmbientOcclusionSamples,

	NumberOfUniformBuffers
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
	uint32 binding;

	//The type.
	Type type;

	//The shader stage.
	ShaderStage shaderStage;

	/*
	*	Constructor taking all values as arguments.
	*/
	RenderDataTableLayoutBinding(const uint32 initialBinding, const Type initialType, const ShaderStage initialShaderStage) NOEXCEPT
		:
		binding(initialBinding),
		type(initialType),
		shaderStage(initialShaderStage)
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
		RenderTarget,
		Texture2D,
		UniformBuffer
	};

	//The binding.
	uint32 binding;

	//The type.
	Type type;

	//The handle.
	OpaqueHandle handle;

	/*
	*	Constructor taking all values as arguments.
	*/
	RenderDataTableUpdateInformation(const uint32 initialBinding, const Type initialType, OpaqueHandle initialHandle) NOEXCEPT
		:
		binding(initialBinding),
		type(initialType),
		handle(initialHandle)
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
		X32Y32Z32W32SignedFloat
	};

	//The location.
	uint32 location;

	//The binding.
	uint32 binding;

	//The format.
	Format format;

	//The offset.
	uint32 offset;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VertexInputAttributeDescription() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VertexInputAttributeDescription(const uint32 initialLocation, const uint32 initialBinding, const Format initialFormat, const uint32 initialOffset)
		:
		location(initialLocation),
		binding(initialBinding),
		format(initialFormat),
		offset(initialOffset)
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
	uint32 binding;

	//The stride.
	uint32 stride;

	//The input rate.
	InputRate inputRate;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VertexInputBindingDescription() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VertexInputBindingDescription(const uint32 initialBinding, const uint32 initialStride, const InputRate initialInputRate)
		:
		binding(initialBinding),
		stride(initialStride),
		inputRate(initialInputRate)
	{

	}

};