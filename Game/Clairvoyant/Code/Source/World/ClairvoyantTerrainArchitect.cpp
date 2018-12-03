//Header file.
#include <World/ClairvoyantTerrainArchitect.h>

//Clairvoyant.
#include <World/BiomeDesert.h>
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoise.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainProperties.h>

//World.
#include <World/ClairvoyantBiomeArchitect.h>

//Clairvoyant terrain generation constants.
namespace ClairvoyantTerrainArchitectConstants
{
	constexpr float TERRAIN_HEIGHT{ 10'000.0f };

	constexpr uint8 TERRAIN_TEST_MATERIAL_INDEX{ 0 };

	constexpr uint8 TERRAIN_CLIFF_SNOW_1_MATERIAL_INDEX{ 1 };
	constexpr uint8 TERRAIN_GRASS_1_MATERIAL_INDEX{ 2 };
	constexpr uint8 TERRAIN_GRASS_2_MATERIAL_INDEX{ 3 };
	constexpr uint8 TERRAIN_SAND_1_MATERIAL_INDEX{ 4 };
	constexpr uint8 TERRAIN_SNOW_1_MATERIAL_INDEX{ 5 };

	constexpr uint8 NUMBER_OF_TERRAIN_MATERIALS{ 6 };
}

/*
*	Initializes the Clairvoyant terrain architect.
*/
void ClairvoyantTerrainArchitect::Initialize() NOEXCEPT
{

}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3<float> &position, float *const RESTRICT height) NOEXCEPT
{
	if (IsWithinTestArea(position))
	{
		*height = ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;

		return;
	}

	//Start off at zero.
	*height = 0.0f;

	//Apply all biomes.
	for (uint8 biome{ 0 }; biome < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++biome)
	{
		const float biomeWeight{ ClairvoyantBiomeArchitect::GetBiomeWeightAtPosition(static_cast<ClairvoyantBiome>(biome), position) };

		if (biomeWeight > 0.0f)
		{
			*height += ClairvoyantBiomeArchitect::GetBiomeHeightAtPosition(static_cast<ClairvoyantBiome>(biome), position) * biomeWeight;
		}
	}

	//Apply the height.
	*height *= ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;
}

/*
*	Generates the material.
*/
void ClairvoyantTerrainArchitect::GenerateMaterial(const TerrainProperties &properties, const Vector3<float> &position, uint8 *const RESTRICT material) NOEXCEPT
{
	if (IsWithinTestArea(position))
	{
		*material = ClairvoyantTerrainArchitectConstants::TERRAIN_TEST_MATERIAL_INDEX;

		return;
	}

	//Calculate the coordinates.
	const float coordinateX{ position._X / 100'000.0f };
	const float coordinateY{ position._Z / 100'000.0f };

	//Calculate the biome weights.
	StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> biomeWeights;

	for (uint8 biome{ 0 }; biome < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++biome)
	{
		biomeWeights[biome] = ClairvoyantBiomeArchitect::GetBiomeWeightAtPosition(static_cast<ClairvoyantBiome>(biome), position);
	}

	//Calculate the material with the highest weight.
	uint8 chosenMaterial{ 0 };
	float highestWeight{ -FLOAT_MAXIMUM };

	//Retrieve the normal.
	Vector3<float> normal;
	TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &normal);

	for (uint8 i{ 1 }; i < ClairvoyantTerrainArchitectConstants::NUMBER_OF_TERRAIN_MATERIALS; ++i)
	{
		const float materialWeight{ GetMaterialWeight(i, coordinateX, coordinateY, biomeWeights, normal) };

		if (highestWeight < materialWeight)
		{
			chosenMaterial = i;
			highestWeight = materialWeight;
		}
	}

	*material = chosenMaterial;
}

/*
*	Returns whether or not a position is within the test area.
*/
bool ClairvoyantTerrainArchitect::IsWithinTestArea(const Vector3<float> &worldPosition) NOEXCEPT
{
	return	CatalystBaseMath::Absolute(worldPosition._X) < 1'000.0f
			&& CatalystBaseMath::Absolute(worldPosition._Z) < 1'000.0f;
}

/*
*	Returns the weight for the given material.
*/
float ClairvoyantTerrainArchitect::GetMaterialWeight(const uint8 material, const float coordinateX, const float coordinateY, const StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> &biomWeights, const Vector3<float> &normal) NOEXCEPT
{
	switch (material)
	{
		case ClairvoyantTerrainArchitectConstants::TERRAIN_CLIFF_SNOW_1_MATERIAL_INDEX:
		{
			return (1.0f - Vector3<float>::DotProduct(normal, Vector3<float>::UP)) * biomWeights[UNDERLYING(ClairvoyantBiome::Snow)];
		}

		case ClairvoyantTerrainArchitectConstants::TERRAIN_GRASS_1_MATERIAL_INDEX:
		{
			return PerlinNoise::GenerateNormalized(coordinateX, coordinateY, 0.0f) * biomWeights[UNDERLYING(ClairvoyantBiome::Grass)];
		}

		case ClairvoyantTerrainArchitectConstants::TERRAIN_GRASS_2_MATERIAL_INDEX:
		{
			return PerlinNoise::GenerateNormalized(coordinateX * 2.0f, coordinateY * 2.0f, 0.0f) * biomWeights[UNDERLYING(ClairvoyantBiome::Grass)];
		}

		case ClairvoyantTerrainArchitectConstants::TERRAIN_SAND_1_MATERIAL_INDEX:
		{
			return biomWeights[UNDERLYING(ClairvoyantBiome::Desert)];
		}

		case ClairvoyantTerrainArchitectConstants::TERRAIN_SNOW_1_MATERIAL_INDEX:
		{
			return Vector3<float>::DotProduct(normal, Vector3<float>::UP) * biomWeights[UNDERLYING(ClairvoyantBiome::Snow)];
		}

		default:
		{
			ASSERT(false, "A case should be added here. ):");

			return 0.0f;
		}
	}
}