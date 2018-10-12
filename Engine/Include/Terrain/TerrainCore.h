#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

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

//Enumeration covering all terrain axis.
enum class TerrainAxis : uint8
{
	None,
	PositiveX,
	NegativeX,
	PositiveY,
	NegativeY
};

DEFINE_ENUM_FLAG_OPERATORS(TerrainAxis);