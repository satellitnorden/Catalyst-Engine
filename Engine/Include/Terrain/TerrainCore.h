#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 10 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr float TERRAIN_PATCH_SIZE{ 65'536.0f };
}

//Forward declarations.
class TerrainProperties;
template <typename Type>
class Vector3;

//Type aliases.
using HeightGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, float *const RESTRICT height);
using MaterialGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, uint8 *const RESTRICT material);