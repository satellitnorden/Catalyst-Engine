#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 NUMBER_OF_TERRAIN_PATCHES{ 57 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 126 };
	constexpr uint32 TERRAIN_PATCH_NORMAL_RESOLUTION{ 64 };
	constexpr float TERRAIN_PATCH_SIZE{ 64.0f };
}

//Forward declarations.
class TerrainMaterial;
class TerrainProperties;
class Vector3;
class Vector4;

//Type aliases.
using HeightGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height);
using LayerWeightsGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights);
using PatchPropertiesGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material);

//Enumeration covering all terrain borders.
enum class TerrainBorder : uint8
{
	None = BIT(0),
	Upper = BIT(1),
	Right = BIT(2),
	Lower = BIT(3),
	Left = BIT(4)
};

ENUMERATION_BIT_OPERATIONS(TerrainBorder);