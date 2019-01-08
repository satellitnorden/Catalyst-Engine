//Header file.
#include <World/ClairvoyantTerrainArchitect.h>

//Clairvoyant.
#include <World/BiomeDesert.h>
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/SimplexNoise.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>

//World.
#include <World/ClairvoyantBiomeArchitect.h>

//Clairvoyant terrain generation constants.
namespace ClairvoyantTerrainArchitectConstants
{
	constexpr float TERRAIN_HEIGHT{ 10'000.0f };

	constexpr float TERRAIN_MINIMUM_FLATNESS{ 0.5f };
	constexpr float TERRAIN_MAXIMUM_FLATNESS{ 1.0f };

	constexpr float TERRAIN_MINIMUM_GAIN{ 0.4f };
	constexpr float TERRAIN_MAXIMUM_GAIN{ 0.6f };

	constexpr float TERRAIN_MINIMUM_HEIGHT_OVER_WATER{ 0.0f };
	constexpr float TERRAIN_MAXIMUM_HEIGHT_OVER_WATER{ 0.5f };

	constexpr uint8 TERRAIN_TEST_MATERIAL_INDEX{ 0 };

	constexpr uint8 TERRAIN_CLIFF_SNOW_1_MATERIAL_INDEX{ 1 };
	constexpr uint8 TERRAIN_GRASS_1_MATERIAL_INDEX{ 2 };
	constexpr uint8 TERRAIN_GRASS_2_MATERIAL_INDEX{ 3 };
	constexpr uint8 TERRAIN_SAND_1_MATERIAL_INDEX{ 4 };
	constexpr uint8 TERRAIN_SNOW_1_MATERIAL_INDEX{ 5 };

	constexpr uint8 NUMBER_OF_TERRAIN_MATERIALS{ 6 };
}

/*
	*	Returns a random offset.
	*/
static float GetRandomOffset(const uint8 index) NOEXCEPT
{
	constexpr float MAXIMUM_OFFSET{ 1.0f };

	switch (index)
	{
		case 0:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 1:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 2:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 3:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 4:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 5:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 6:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 7:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 8:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 9:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 10:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 11:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 12:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 13:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 14:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 15:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 16:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 17:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 18:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 19:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 20:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 21:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 22:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 23:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 24:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}
	}

	ASSERT(false, "A case should be added here. ):");

	return 0.0f;
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
	//Define constants.
	constexpr float LACUNARITY{ 2.0f };
	constexpr uint8 OCTAVES{ 10 };

	//Calculate the coordinate.
	const Vector2<float> coordinate{ position._X / 100'000.0f, position._Z / 100'000.0f };

	//Generate properties.
	const float flatness{ CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainArchitectConstants::TERRAIN_MINIMUM_FLATNESS, ClairvoyantTerrainArchitectConstants::TERRAIN_MAXIMUM_FLATNESS, (SimplexNoise::GenerateDerivaties(coordinate, GetRandomOffset(0))._X + 1.0f) * 0.5f) };
	const float gain{ CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainArchitectConstants::TERRAIN_MINIMUM_GAIN, ClairvoyantTerrainArchitectConstants::TERRAIN_MAXIMUM_GAIN, (SimplexNoise::GenerateDerivaties(coordinate, GetRandomOffset(1))._X + 1.0f) * 0.5f) };
	const float heighOverWater{ CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainArchitectConstants::TERRAIN_MINIMUM_HEIGHT_OVER_WATER, ClairvoyantTerrainArchitectConstants::TERRAIN_MAXIMUM_HEIGHT_OVER_WATER, (SimplexNoise::GenerateDerivaties(coordinate, GetRandomOffset(2))._X + 1.0f) * 0.5f) };
	//Start off at zero.
	*height = 0.0f;

	//Apply the octaves.
	Vector2<float> derivaties{ 0.0f, 0.0f };

	float amplitude{ 1.0f };
	float frequency{ 1.0f };

	for (uint8 i{ 0 }; i < OCTAVES; ++i)
	{
		Vector3<float> noise{ SimplexNoise::GenerateDerivaties(coordinate * frequency, GetRandomOffset(i + 3)) };

		derivaties._X += noise._Y;
		derivaties._Y += noise._Z;

		*height += amplitude * noise._X / (1.0f + derivaties._X * derivaties._X + derivaties._Y * derivaties._Y);

		frequency *= LACUNARITY;
		amplitude *= gain;
	}

	//Apply the height.
	*height = (*height * flatness + heighOverWater) * ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;
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