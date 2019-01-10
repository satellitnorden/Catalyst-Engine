//Header file.
#include <World/ClairvoyantBiomeArchitect.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/StaticArray.h>

//Clairvoyant.
#include <World/BiomeKingsmeadow.h>

//Math.
#include <Math/PerlinNoise.h>

namespace ClairvoyantBiomeArchitectConstants
{
	constexpr float BIOME_SIZE{ 100'000.0f };
}

/*
*	Returns the biome weight at the given position.
*/
void ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT
{
	weights->At(UNDERLYING(ClairvoyantBiome::Crossroads)) = 1.0f;

	/*
	//Calculate the biome step.
	constexpr float BIOME_STEP{ 1.0f / static_cast<float>(UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes) - 1) };

	//Calculate the random offset.
	static const float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1'000.0f) };

	//Calculate the coordinates.
	const float coordinateX{ position._X / (ClairvoyantBiomeArchitectConstants::BIOME_SIZE) };
	const float coordinateY{ position._Z / (ClairvoyantBiomeArchitectConstants::BIOME_SIZE) };

	//Calculate the noise.
	const float noise{ CatalystBaseMath::SmoothStep<1>(PerlinNoise::GenerateNormalized(coordinateX, coordinateY, randomOffset)) };

	//Calculate the weights.
	for (uint8 biome{ 0 }; biome < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++biome)
	{
		weights->At(biome) = 1.0f - (CatalystBaseMath::Minimum<float>(CatalystBaseMath::Absolute((static_cast<float>(biome) / static_cast<float>(UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes) - 1)) - noise) / BIOME_STEP, 1.0f));
	}
	*/
}

/*
*	Returns the material of the given biome of that given position.
*/
uint8 ClairvoyantBiomeArchitect::GetBiomeMaterialAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::Crossroads:
		{
			return BiomeKingsmeadow::Material(position);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0;
		}
	}
}