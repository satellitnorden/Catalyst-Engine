#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint32 SHADOW_MAP_RESOLUTION{ 2'048 };
	constexpr float SHADOW_VIEW_DISTANCE{ 1'000.0f };
}

/*
*	Type aliases.
*/
using DescriptorSetHandle = void *RESTRICT;
using ConstantBufferHandle = void *RESTRICT;
using EventHandle = void *RESTRICT;
using Texture2DHandle = void *RESTRICT;
using TextureCubeMapHandle = void *RESTRICT;
using UniformBufferHandle = void *RESTRICT;

//Enumeration covering all render passes stages.
enum class RenderPassStage : uint8
{
	DirectionalTerrainShadow,
	DirectionalStaticPhysicalShadow,
	DirectionalInstancedPhysicalShadow,
	Terrain,
	StaticPhysical,
	InstancedPhysical,
	Vegetation,
	DirectionalShadow,
	ShadowBlur,
	Lighting,
	Sky,
	Ocean,
	ParticleSystem,
	PostProcessing,

	NumberOfRenderPassStages
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

//Enumeration covering all descriptor set layouts.
enum class DescriptorSetLayout : uint8
{
	DynamicUniformData,
	ShadowMapBlur,
	Environment,
	Terrain,
	Physical,
	Vegetation,
	DirectionalShadow,
	Lighting,
	Ocean,
	ParticleSystem,
	PostProcessing,
	NumberOfDescriptorSetLayouts
};

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	DirectionalShadowMap,
	DirectionalPreBlurShadowMap,
	DirectionalPostBlurShadowMap,
	SceneBufferAlbedo,
	SceneBufferNormalDepth,
	SceneBufferMaterialProperties,
	Scene,
	WaterScene,
	Screen,
	NumberOfRenderTargets
};

//Enumeration covering all shaders.
enum class Shader : uint8
{
	CubeMapFragment,
	CubeMapVertex,
	DirectionalPhysicalShadowVertex,
	DirectionalShadowFragment,
	DirectionalInstancedPhysicalShadowVertex,
	DirectionalTerrainShadowTessellationEvaluation,
	InstancedPhysicalVertex,
	LightingFragment,
	OceanFragment,
	ParticleSystemFragment,
	ParticleSystemGeometry,
	ParticleSystemVertex,
	PostProcessingFragment,
	PhysicalFragment,
	PhysicalVertex,
	ShadowBlurFragment,
	ShadowMapFragment,
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
*	Push constant range.
*/
class PushConstantRange final
{

public:

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