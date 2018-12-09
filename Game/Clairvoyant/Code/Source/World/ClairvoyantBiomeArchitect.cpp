//Header file.
#include <World/ClairvoyantBiomeArchitect.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/PerlinNoise.h>

//World.
#include <World/BiomeDesert.h>
#include <World/BiomeGrass.h>
#include <World/BiomeSnow.h>

namespace ClairvoyantBiomeArchitectConstants
{
	constexpr float BIOME_SIZE{ 100'000.0f };
}

/*
*	Returns the biome weight at the given position.
*/
void ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT
{
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
		if (true)
		{
			weights->At(biome) = biome == UNDERLYING(ClairvoyantBiome::Desert) ? 1.0f : 0.0f;
		}

		else
		{
			weights->At(biome) = 1.0f - (CatalystBaseMath::Minimum<float>(CatalystBaseMath::Absolute((static_cast<float>(biome) / static_cast<float>(UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes) - 1)) - noise) / BIOME_STEP, 1.0f));
		}
	}
}

/*
*	Returns the biome height at the given position.
*/
float ClairvoyantBiomeArchitect::GetBiomeHeightAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::Desert:
		{
			return BiomeDesert::Height(position);
		}

		case ClairvoyantBiome::Grass:
		{
			return BiomeGrass::Height(position);
		}

		case ClairvoyantBiome::Snow:
		{
			return BiomeSnow::Height(position);
		}

		default:
		{
			ASSERT(false, "There should probably be a case here. ):");

			return 0.0f;
		}
	}
}