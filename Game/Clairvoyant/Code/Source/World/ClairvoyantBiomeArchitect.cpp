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
float ClairvoyantBiomeArchitect::GetBiomeWeightAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT
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

	//Calculate the biome value.
	const float biomeValue{ static_cast<float>(UNDERLYING(biome)) / static_cast<float>(UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes) - 1) };

	//Calculate the biome weight.
	return CatalystBaseMath::SmoothStep<2>(1.0f - CatalystBaseMath::Minimum<float>(CatalystBaseMath::Absolute(biomeValue - noise) / BIOME_STEP, 1.0f));
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