//Header file.
#include <World/ClairvoyantTerrainArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Matrix2.h>
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

	constexpr float TERRAIN_MINIMUM_HEIGHT_OVER_WATER{ 0.0f };
	constexpr float TERRAIN_MAXIMUM_HEIGHT_OVER_WATER{ 0.5f };
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
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Grass_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Grass_1_Material")), 0.5f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Rock_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Rock_1_Material")), 0.5f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Sand_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Sand_1_Material")), 0.5f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Snow_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Snow_1_Material")), 0.5f);
}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3<float> &position, float *const RESTRICT height) NOEXCEPT
{
	//Define constants.
	constexpr float GAIN{ 0.5f };
	constexpr float NOISE_SCALE{ 100'000.0f };
	constexpr Matrix2 TRANSFORMATION{ 0.8f, -0.6f, 0.6f, 0.8f };
	constexpr uint8 OCTAVES{ 12 };

	//Calculate the coordinate.
	Vector2<float> coordinate{ position._X / NOISE_SCALE, position._Z / NOISE_SCALE };

	//Generate properties.
	const float flatness{ CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainArchitectConstants::TERRAIN_MINIMUM_FLATNESS, ClairvoyantTerrainArchitectConstants::TERRAIN_MAXIMUM_FLATNESS, (SimplexNoise::Generate(coordinate, GetRandomOffset(0)) + 1.0f) * 0.5f) };
	const float heighOverWater{ CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainArchitectConstants::TERRAIN_MINIMUM_HEIGHT_OVER_WATER, ClairvoyantTerrainArchitectConstants::TERRAIN_MAXIMUM_HEIGHT_OVER_WATER, (SimplexNoise::Generate(coordinate, GetRandomOffset(1)) + 1.0f) * 0.5f) };
	
	//Start off at zero.
	*height = 0.0f;

	//Apply the octaves.
	Vector2<float> derivaties{ 0.0f, 0.0f };

	float amplitude{ 1.0f };
	float frequency{ 1.0f };

	for (uint8 i{ 0 }; i < OCTAVES; ++i)
	{
		Vector3<float> noise{ SimplexNoise::GenerateDerivaties(coordinate, GetRandomOffset(i + 2)) };

		derivaties._X += noise._Y;
		derivaties._Y += noise._Z;

		*height += amplitude * noise._X / (1.0f + derivaties._X * derivaties._X + derivaties._Y * derivaties._Y);

		amplitude *= GAIN;

		coordinate = TRANSFORMATION * coordinate * 2.0f;
	}

	//Apply the height.
	*height = (*height * flatness + heighOverWater) * ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;
}

/*
*	Generates the material.
*/
void ClairvoyantTerrainArchitect::GenerateMaterial(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material) NOEXCEPT
{
	StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> biomeWeights;
	ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(worldPosition, &biomeWeights);

	Pair<float, uint8> highestBiomeWeight{ -FLOAT_MAXIMUM, 0 };

	for (uint8 i{ 0 }; i < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++i)
	{
		if (biomeWeights[i] > highestBiomeWeight._First)
		{
			highestBiomeWeight._First = biomeWeights[i];
			highestBiomeWeight._Second = ClairvoyantBiomeArchitect::GetBiomeMaterialAtPosition(static_cast<ClairvoyantBiome>(i), worldPosition, height, normal);
		}
	}

	*material = highestBiomeWeight._Second;
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
	return 0.0f;
}