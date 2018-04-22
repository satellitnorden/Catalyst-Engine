#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>

//Rendering constants.
namespace RenderingConstants
{
	constexpr uint32 SHADOW_MAP_RESOLUTION{ 2'048 };
	constexpr float SHADOW_VIEW_DISTANCE{ 250.0f };
}

/*
*	Type aliases.
*/
using DescriptorSetHandle = VulkanDescriptorSet;
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