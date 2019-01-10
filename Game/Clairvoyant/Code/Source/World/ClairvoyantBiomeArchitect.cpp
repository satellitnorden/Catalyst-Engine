//Header file.
#include <World/ClairvoyantBiomeArchitect.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/StaticArray.h>

//Clairvoyant.
#include <World/BiomeKingsMeadow.h>
#include <World/BiomeNorthHollow.h>

//Math.
#include <Math/PerlinNoise.h>

namespace ClairvoyantBiomeArchitectConstants
{
	constexpr float BIOME_SIZE{ 100'000.0f };
}

/*
*	Returns the biome weight for the given biome at the given position.
*/
float ClairvoyantBiomeArchitect::GetBiomeWeightAtPosition(const Vector3<float> &position, const ClairvoyantBiome biome) NOEXCEPT
{
	//Get the height at the position.
	float height;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &height);

	//Start off at zero.
	float weight{ 0.0f };

	//Add the height increase.
	weight += GetBiomeHeightWeightIncrease(biome, height);

	//Return the weight.
	return weight;
}

/*
*	Returns the biome height weight increase for the given biome.
*/
float ClairvoyantBiomeArchitect::GetBiomeHeightWeightIncrease(const ClairvoyantBiome biome, const float height) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::KingsMeadow:
		{
			return BiomeKingsMeadow::HeightWeightIncrease(height);
		}

		case ClairvoyantBiome::NorthHollow:
		{
			return BiomeNorthHollow::HeightWeightIncrease(height);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0.0f;
		}
	}
}

/*
*	Returns the biome weight at the given position.
*/
void ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT
{
	for (uint8 i{ 0 }; i < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++i)
	{
		weights->At(i) = GetBiomeWeightAtPosition(position, static_cast<ClairvoyantBiome>(i));
	}
}

/*
*	Returns the material of the given biome of that given position.
*/
uint8 ClairvoyantBiomeArchitect::GetBiomeMaterialAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::KingsMeadow:
		{
			return BiomeKingsMeadow::Material(position);
		}

		case ClairvoyantBiome::NorthHollow:
		{
			return BiomeNorthHollow::Material(position);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0;
		}
	}
}