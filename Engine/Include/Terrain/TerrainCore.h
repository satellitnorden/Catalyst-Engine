#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr int32 TERRAIN_BORDER_UPPER{ BIT(0) };
	constexpr int32 TERRAIN_BORDER_RIGHT{ BIT(1) };
	constexpr int32 TERRAIN_BORDER_LOWER{ BIT(2) };
	constexpr int32 TERRAIN_BORDER_LEFT{ BIT(3) };
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 10 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr float TERRAIN_PATCH_SIZE{ 65'536.0f };
}

//Forward declarations.
class TerrainMaterial;
class TerrainProperties;
class Vector3;
class Vector4;

//Type aliases.
using HeightGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height);
using LayerWeightsGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, Vector4 *const RESTRICT layerWeights);
using PatchPropertiesGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material);