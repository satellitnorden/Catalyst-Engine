//Header file.
#include <World/ClairvoyantTerrainArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainProperties.h>

//Clairvoyant terrain generation constants.
namespace ClairvoyantTerrainGenerationConstants
{
	constexpr float TERRAIN_HEIGHT{ 10'000.0f };
}

/*
*	Returns the random offset.
*/
float GetRandomOffset(const uint8 index) NOEXCEPT
{
	constexpr float MAXIMUM_OFFSET{ 1'000.0f };

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
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

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

		case 25:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}
	}

	ASSERT(false, "You should add a case here. ):");

	return 0.0f;
}

/*
*	Initializes the Clairvoyant terrain architect.
*/
void ClairvoyantTerrainArchitect::Initialize() NOEXCEPT
{
	//Register the terrain materials.
	TerrainSystem::Instance->RegisterTerrainMaterial(0, ResourceLoader::GetPhysicalMaterial(HashString("TerrainSandMaterial")));
	TerrainSystem::Instance->RegisterTerrainMaterial(1, ResourceLoader::GetPhysicalMaterial(HashString("TerrainSandRockMaterial")));
}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT
{
	//Calculate the coordinates.
	const float coordinateX{ worldPosition._X / 100'000.0f };
	const float coordinateY{ worldPosition._Z / 100'000.0f };

	//Base height.
	*height = 0.0f;

	*height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX, coordinateY, GetRandomOffset(0));
	*height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX * 2.0f, coordinateY * 2.0f, GetRandomOffset(1)) * 0.5f;
	*height += PerlinNoiseGenerator::GenerateNoise(coordinateX * 4.0f, coordinateY * 4.0f, GetRandomOffset(2)) * 0.25f;
	*height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 8.0f, coordinateY * 8.0f, GetRandomOffset(3)) * 0.125f;
	*height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 16.0f, coordinateY * 16.0f, GetRandomOffset(4)) * 0.0625f;

	//Apply the height.
	*height *= ClairvoyantTerrainGenerationConstants::TERRAIN_HEIGHT;
}

/*
*	Generates the material.
*/
void ClairvoyantTerrainArchitect::GenerateMaterial(const TerrainProperties &properties, const Vector3 &worldPosition, uint8 *const RESTRICT material) NOEXCEPT
{
	//Calculate the coordinates.
	const float coordinateX{ worldPosition._X / 10'000.0f };
	const float coordinateY{ worldPosition._Z / 10'000.0f };

	//General landscape shape.
	if (PerlinNoiseGenerator::GenerateNoise(coordinateX, coordinateY, GetRandomOffset(0)) > 0.0f)
	{
		*material = 0;
	}

	else
	{
		*material = 0;
	}
}