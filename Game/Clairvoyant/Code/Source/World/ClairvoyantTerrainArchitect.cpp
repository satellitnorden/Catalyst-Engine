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
	//Register all terrain materials.
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Grass_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Grass_1_Material")), 0.2f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Grass_2), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Grass_2_Material")), 0.2f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Sand_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Sand_1_Material")), 0.2f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Sand_2), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Sand_2_Material")), 0.2f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Sand_3), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Sand_3_Material")), 0.2f);
}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3<float> &position, float *const RESTRICT height) NOEXCEPT
{
	//Start off at zero.
	*height = 0.0f;

	//Retrieve the biome weights.
	StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> weights;

	ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(position, &weights);

	//Apply all biomes.
	for (uint8 biome{ 0 }; biome < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++biome)
	{
		if (weights[biome] > 0.0f)
		{
			*height += ClairvoyantBiomeArchitect::GetBiomeHeightAtPosition(static_cast<ClairvoyantBiome>(biome), position) * weights[biome];
		}
	}

	//Apply the height.
	*height *= ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;
}

/*
*	Generates the material.
*/
void ClairvoyantTerrainArchitect::GenerateMaterial(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material) NOEXCEPT
{
	*material = UNDERLYING(ClairvoyantTerrainMaterial::Grass_2);
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