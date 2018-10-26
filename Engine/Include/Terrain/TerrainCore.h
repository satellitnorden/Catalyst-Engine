#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 NUMBER_OF_TERRAIN_PATCHES{ 65 };
}

//Forward declarations.
class TerrainDisplacementInformation;
class TerrainMaterial;
class TerrainProperties;
class Vector3;
class Vector4;

//Type aliases.
using HeightGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height);
using LayerWeightsGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights);
using PatchPropertiesGenerationFunction = void(*)(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material, TerrainDisplacementInformation *const RESTRICT displacementInformation);

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