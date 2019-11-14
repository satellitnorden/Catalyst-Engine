#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 MAXIMUM_NUMBER_OF_TERRAIN_PATCHES{ UINT8_MAXIMUM };
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 6 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr uint32 TERRAIN_HEIGHT_TEXTURE_RESOLUTION{ TERRAIN_PATCH_RESOLUTION };
	constexpr uint32 TERRAIN_TEXTURE_BASE_RESOLUTION{ 64 };
	constexpr uint32 TERRAIN_TEXTURE_MAXIMUM_RESOLUTION{ 1'024 };
	constexpr float TERRAIN_PATCH_SIZE{ 4'096.0f };
}

//Forward declarations.
class TerrainProperties;

//Type aliases.
using HeightFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &world_position, float *const RESTRICT height, const void *const RESTRICT context);
using MaterialFunction = void(*)(const TerrainProperties &properties, const Vector3<float> & world_position, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material, const void* const RESTRICT context);