#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_TERRAIN_PATCHES{ 255 };
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 6 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr uint32 TERRAIN_HEIGHT_TEXTURE_RESOLUTION{ TERRAIN_PATCH_RESOLUTION };
	constexpr uint32 TERRAIN_TEXTURE_BASE_RESOLUTION{ 64 };
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_TEXTURE_RESOLUTIONS
	{
		TERRAIN_TEXTURE_BASE_RESOLUTION * 64,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 32,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 16,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 8,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 4,
		TERRAIN_TEXTURE_BASE_RESOLUTION * 2,
		TERRAIN_TEXTURE_BASE_RESOLUTION
	};
	constexpr float TERRAIN_PATCH_SIZE{ 4'096.0f };
}

//Forward declarations.
class TerrainProperties;

//Type aliases.
using HeightFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &world_position, float *const RESTRICT height, const void *const RESTRICT context);
using MaterialFunction = void(*)(const TerrainProperties &properties, const Vector3<float> & world_position, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material, const void* const RESTRICT context);