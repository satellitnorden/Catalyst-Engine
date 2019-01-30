#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 10 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr uint32 TERRAIN_HEIGHT_TEXTURE_RESOLUTION{ TERRAIN_PATCH_RESOLUTION };
	constexpr uint32 TERRAIN_TEXTURE_BASE_RESOLUTION{ 64 };
#if defined(CATALYST_CONFIGURATION_DEBUG)
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_TEXTURE_RESOLUTIONS
	{
		TERRAIN_TEXTURE_BASE_RESOLUTION * 16,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 16,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 8,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 8,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 8,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 4,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 4,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 4,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 2,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 2,
		TERRAIN_TEXTURE_BASE_RESOLUTION
	};
#else
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_TEXTURE_RESOLUTIONS
	{
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION,
		TERRAIN_TEXTURE_BASE_RESOLUTION
	};
#endif
	constexpr float TERRAIN_PATCH_SIZE{ 65'536.0f };
}

//Forward declarations.
class TerrainProperties;

//Type aliases.
using HeightFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, float *const RESTRICT height);
using MaterialFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material);