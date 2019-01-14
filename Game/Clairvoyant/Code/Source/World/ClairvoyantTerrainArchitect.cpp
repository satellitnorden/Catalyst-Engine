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
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Cliff_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Cliff_1_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Cliff_2), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Cliff_2_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Cliff_3), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Cliff_3_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Cliff_4), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Cliff_4_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Grass_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Grass_1_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Grass_2), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Grass_2_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Leaves_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Leaves_1_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Roots_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Roots_1_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Sand_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Sand_1_Material")), 0.25f);
	TerrainSystem::Instance->RegisterTerrainMaterial(UNDERLYING(ClairvoyantTerrainMaterial::Snow_1), ResourceLoader::GetPhysicalMaterial(HashString("Terrain_Snow_1_Material")), 0.25f);
}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3<float> &position, float *const RESTRICT height) NOEXCEPT
{
	//Define constants.
	constexpr float PROPERTIES_NOISE_SCALE{ 100'000.0f };
	constexpr float LANDSCAPE_NOISE_SCALE{ 100'000.0f };
	constexpr float DETAIL_NOISE_SCALE{ 10'000.0f };
	constexpr Matrix2 TRANSFORMATION{ 0.8f, -0.6f, 0.6f, 0.8f };
	constexpr uint8 LANDSCAPE_OCTAVES{ 5 };
	constexpr uint8 DETAIL_OCTAVES{ 7 };

	//Generate the properties.
	float landscapeAmplitude;
	float landscapeGain;
	float landscapeSmoothness;
	float detailAmplitude;
	float detailGain;
	float heighOverWater;

	{
		Vector2<float> coordinate{ position._X / PROPERTIES_NOISE_SCALE, position._Z / PROPERTIES_NOISE_SCALE };

		landscapeAmplitude = CatalystBaseMath::LinearlyInterpolate(0.0f, 1.0f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(0)));
		landscapeGain = CatalystBaseMath::LinearlyInterpolate(0.4f, 0.6f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(1)));
		landscapeSmoothness = CatalystBaseMath::LinearlyInterpolate(0.0f, 1.0f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(2)));
		detailAmplitude = CatalystBaseMath::LinearlyInterpolate(0.0f, 0.25f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(3)));
		detailGain = CatalystBaseMath::LinearlyInterpolate(0.475f, 0.525f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(4)));
		heighOverWater = CatalystBaseMath::LinearlyInterpolate(-0.125f, 0.075f, SimplexNoise::GenerateNormalized(coordinate, GetRandomOffset(5)));
	}

	//Start off at zero.
	*height = 0.0f;

	//Apply the landscape noise.
	{
		Vector2<float> coordinate{ position._X / LANDSCAPE_NOISE_SCALE, position._Z / LANDSCAPE_NOISE_SCALE };

		float amplitude{ landscapeAmplitude };
		float frequency{ 1.0f };

		for (uint8 i{ 0 }; i < LANDSCAPE_OCTAVES; ++i)
		{
			const float noise{ SimplexNoise::GenerateNormalized(coordinate * frequency, GetRandomOffset(i + 5)) };

			*height += CatalystBaseMath::LinearlyInterpolate(noise, CatalystBaseMath::SmoothStep<3>(noise), landscapeSmoothness) * amplitude;

			amplitude *= landscapeGain;
			frequency *= 2.0f;
		}
	}

	//Apply the detail noise.
	{
		Vector2<float> coordinate{ position._X / DETAIL_NOISE_SCALE, position._Z / DETAIL_NOISE_SCALE };

		Vector2<float> derivaties{ 0.0f, 0.0f };

		float amplitude{ detailAmplitude };

		for (uint8 i{ 0 }; i < DETAIL_OCTAVES; ++i)
		{
			Vector3<float> noise{ SimplexNoise::GenerateDerivaties(coordinate, GetRandomOffset(i + 5 + LANDSCAPE_OCTAVES)) };

			derivaties._X += noise._Y;
			derivaties._Y += noise._Z;

			*height += amplitude * noise._X / (1.0f + derivaties._X * derivaties._X + derivaties._Y * derivaties._Y);

			amplitude *= detailGain;

			coordinate = TRANSFORMATION * coordinate * 2.0f;
		}
	}

	//Apply the height.
	*height = (*height + heighOverWater) * ClairvoyantTerrainArchitectConstants::TERRAIN_HEIGHT;
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