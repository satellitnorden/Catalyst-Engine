#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint32 SHADOW_MAP_RESOLUTION{ 2'048 };
	constexpr float SHADOW_VIEW_DISTANCE{ 2'500.0f };
}

//Enumeration covering all render passes stages.
enum class RenderPassStage : uint8
{
	Terrain,

	NumberOfRenderPasses
};

//Enumeration covering all shaders.
enum class Shader : uint8
{
	SceneBufferFragment,
	TerrainFragment,
	TerrainTessellationControl,
	TerrainTessellationEvaluation,
	TerrainVertex,

	NumberOfShaders,
	
	None
};

//Enumeration covering all depth buffers.
enum class DepthBuffer : uint8
{
	SceneBuffer,

	NumberOfDepthBuffers,

	None
};

//Enumeration covering all render targets.
enum class RenderTarget : uint8
{
	SceneBufferAlbedo,
	SceneBufferNormalDepth,
	SceneBufferMaterialProperties,
	NumberOfRenderTargets
};

//Enumeration covering all cull faces.
enum class CullFace : uint8
{
	None,
	Back,
	Front
};

/*
*	Type aliases.
*/
using DescriptorSetHandle = void *RESTRICT;
using GraphicsBufferHandle = void *RESTRICT;
using Texture2DHandle = void *RESTRICT;
using TextureCubeMapHandle = void *RESTRICT;
using UniformBufferHandle = void *RESTRICT;

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