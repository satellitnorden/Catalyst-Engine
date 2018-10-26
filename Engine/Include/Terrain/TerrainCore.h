#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 NUMBER_OF_TERRAIN_PATCHES{ 65 };

	constexpr uint8 CENTER_INDEX{ 0 };

	constexpr uint8 UPPER_LAYER_ZERO_INDEX{ 1 };
	constexpr uint8 UPPER_RIGHT_LAYER_ZERO_INDEX{ 2 };
	constexpr uint8 RIGHT_LAYER_ZERO_INDEX{ 3 };
	constexpr uint8 LOWER_RIGHT_LAYER_ZERO_INDEX{ 4 };
	constexpr uint8 LOWER_LAYER_ZERO_INDEX{ 5 };
	constexpr uint8 LOWER_LEFT_LAYER_ZERO_INDEX{ 6 };
	constexpr uint8 LEFT_LAYER_ZERO_INDEX{ 7 };
	constexpr uint8 UPPER_LEFT_LAYER_ZERO_INDEX{ 8 };

	constexpr uint8 UPPER_LAYER_ONE_INDEX{ 9 };
	constexpr uint8 UPPER_RIGHT_LAYER_ONE_INDEX{ 10 };
	constexpr uint8 RIGHT_LAYER_ONE_INDEX{ 11 };
	constexpr uint8 LOWER_RIGHT_LAYER_ONE_INDEX{ 12 };
	constexpr uint8 LOWER_LAYER_ONE_INDEX{ 13 };
	constexpr uint8 LOWER_LEFT_LAYER_ONE_INDEX{ 14 };
	constexpr uint8 LEFT_LAYER_ONE_INDEX{ 15 };
	constexpr uint8 UPPER_LEFT_LAYER_ONE_INDEX{ 16 };

	constexpr uint8 UPPER_LAYER_TWO_INDEX{ 17 };
	constexpr uint8 UPPER_RIGHT_LAYER_TWO_INDEX{ 18 };
	constexpr uint8 RIGHT_LAYER_TWO_INDEX{ 19 };
	constexpr uint8 LOWER_RIGHT_LAYER_TWO_INDEX{ 20 };
	constexpr uint8 LOWER_LAYER_TWO_INDEX{ 21 };
	constexpr uint8 LOWER_LEFT_LAYER_TWO_INDEX{ 22 };
	constexpr uint8 LEFT_LAYER_TWO_INDEX{ 23 };
	constexpr uint8 UPPER_LEFT_LAYER_TWO_INDEX{ 24 };

	constexpr uint8 UPPER_LAYER_THREE_INDEX{ 25 };
	constexpr uint8 UPPER_RIGHT_LAYER_THREE_INDEX{ 26 };
	constexpr uint8 RIGHT_LAYER_THREE_INDEX{ 27 };
	constexpr uint8 LOWER_RIGHT_LAYER_THREE_INDEX{ 28 };
	constexpr uint8 LOWER_LAYER_THREE_INDEX{ 29 };
	constexpr uint8 LOWER_LEFT_LAYER_THREE_INDEX{ 30 };
	constexpr uint8 LEFT_LAYER_THREE_INDEX{ 31 };
	constexpr uint8 UPPER_LEFT_LAYER_THREE_INDEX{ 32 };

	constexpr uint8 UPPER_LAYER_FOUR_INDEX{ 33 };
	constexpr uint8 UPPER_RIGHT_LAYER_FOUR_INDEX{ 34 };
	constexpr uint8 RIGHT_LAYER_FOUR_INDEX{ 35 };
	constexpr uint8 LOWER_RIGHT_LAYER_FOUR_INDEX{ 36 };
	constexpr uint8 LOWER_LAYER_FOUR_INDEX{ 37 };
	constexpr uint8 LOWER_LEFT_LAYER_FOUR_INDEX{ 38 };
	constexpr uint8 LEFT_LAYER_FOUR_INDEX{ 39 };
	constexpr uint8 UPPER_LEFT_LAYER_FOUR_INDEX{ 40 };

	constexpr uint8 UPPER_LAYER_FIVE_INDEX{ 41 };
	constexpr uint8 UPPER_RIGHT_LAYER_FIVE_INDEX{ 42 };
	constexpr uint8 RIGHT_LAYER_FIVE_INDEX{ 43 };
	constexpr uint8 LOWER_RIGHT_LAYER_FIVE_INDEX{ 44 };
	constexpr uint8 LOWER_LAYER_FIVE_INDEX{ 45 };
	constexpr uint8 LOWER_LEFT_LAYER_FIVE_INDEX{ 46 };
	constexpr uint8 LEFT_LAYER_FIVE_INDEX{ 47 };
	constexpr uint8 UPPER_LEFT_LAYER_FIVE_INDEX{ 48 };

	constexpr uint8 UPPER_LAYER_SIX_INDEX{ 49 };
	constexpr uint8 UPPER_RIGHT_LAYER_SIX_INDEX{ 50 };
	constexpr uint8 RIGHT_LAYER_SIX_INDEX{ 51 };
	constexpr uint8 LOWER_RIGHT_LAYER_SIX_INDEX{ 52 };
	constexpr uint8 LOWER_LAYER_SIX_INDEX{ 53 };
	constexpr uint8 LOWER_LEFT_LAYER_SIX_INDEX{ 54 };
	constexpr uint8 LEFT_LAYER_SIX_INDEX{ 55 };
	constexpr uint8 UPPER_LEFT_LAYER_SIX_INDEX{ 56 };

	constexpr uint8 UPPER_LAYER_SEVEN_INDEX{ 57 };
	constexpr uint8 UPPER_RIGHT_LAYER_SEVEN_INDEX{ 58 };
	constexpr uint8 RIGHT_LAYER_SEVEN_INDEX{ 59 };
	constexpr uint8 LOWER_RIGHT_LAYER_SEVEN_INDEX{ 60 };
	constexpr uint8 LOWER_LAYER_SEVEN_INDEX{ 61 };
	constexpr uint8 LOWER_LEFT_LAYER_SEVEN_INDEX{ 62 };
	constexpr uint8 LEFT_LAYER_SEVEN_INDEX{ 63 };
	constexpr uint8 UPPER_LEFT_LAYER_SEVEN_INDEX{ 64 };
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

//Enumeration covering all terrain axis.
enum class TerrainAxis : uint8
{
	None = BIT(0),
	PositiveX = BIT(1),
	NegativeX = BIT(2),
	PositiveY = BIT(3),
	NegativeY = BIT(4)
};

ENUMERATION_BIT_OPERATIONS(TerrainAxis);