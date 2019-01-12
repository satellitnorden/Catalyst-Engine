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
	constexpr float BIOME_PROPERTIES_SCALE{ 100'000.0f };
	constexpr float BIOME_HEIGHT_SCALE{ 10'000.0f };
}

/*
*	Returns the biome weight at the given position.
*/
void ClairvoyantBiomeArchitect::GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT
{
	//Retrieve the biome properties at the given position.
	BiomeProperties properties;
	GetBiomePropertiesAtPosition(position, &properties);

	for (uint8 i{ 0 }; i < UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes); ++i)
	{
		weights->At(i) = GetBiomeWeightAtPosition(properties, static_cast<ClairvoyantBiome>(i));
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

/*
*	Returns the biome properties at the given position.
*/
void ClairvoyantBiomeArchitect::GetBiomePropertiesAtPosition(const Vector3<float> &position, BiomeProperties *const RESTRICT properties) NOEXCEPT
{
	//Calculate the coordinate.
	Vector2<float> coordinate{ position._X / ClairvoyantBiomeArchitectConstants::BIOME_PROPERTIES_SCALE , position._Z / ClairvoyantBiomeArchitectConstants::BIOME_PROPERTIES_SCALE };

	//Calculate the properties.
	static float precipitationSeed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
	properties->_Precipitation = SimplexNoise::GenerateNormalized(coordinate, precipitationSeed);

	static float temperatureSeed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
	properties->_Temperature = SimplexNoise::GenerateNormalized(coordinate, temperatureSeed);

	//Modify the properties based on the height of the terrain.
	float height;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &height);

	const float heightFactor{ CatalystBaseMath::Minimum<float>(height / ClairvoyantBiomeArchitectConstants::BIOME_HEIGHT_SCALE, 1.0f) };

	properties->_Precipitation = properties->_Precipitation * (1.0f - heightFactor);
	properties->_Temperature = properties->_Temperature * (1.0f - heightFactor);
}

/*
*	Returns the biome weight for the given biome with the given biome properties.
*/
float ClairvoyantBiomeArchitect::GetBiomeWeightAtPosition(const BiomeProperties &properties, const ClairvoyantBiome biome) NOEXCEPT
{
	//Calculate the weight.
	return	GetBiomePrecipitationWeightIncrease(properties._Precipitation, GetBiomeIdealPrecipitation(biome))
			+ GetBiomeTemperatureWeightIncrease(properties._Temperature, GetBiomeIdealTemperature(biome));
}


/*
*	Returns the ideal precipitation of the given biome.
*/
float ClairvoyantBiomeArchitect::GetBiomeIdealPrecipitation(const ClairvoyantBiome biome) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::KingsMeadow:
		{
			return BiomeKingsMeadow::IdealPrecipitation();
		}

		case ClairvoyantBiome::NorthHollow:
		{
			return BiomeNorthHollow::IdealPrecipitation();
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0.0f;
		}
	}
}

/*
*	Returns the ideal temperature of the given biome.
*/
float ClairvoyantBiomeArchitect::GetBiomeIdealTemperature(const ClairvoyantBiome biome) NOEXCEPT
{
	switch (biome)
	{
		case ClairvoyantBiome::KingsMeadow:
		{
			return BiomeKingsMeadow::IdealTemperature();
		}

		case ClairvoyantBiome::NorthHollow:
		{
			return BiomeNorthHollow::IdealTemperature();
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0.0f;
		}
	}
}

/*
*	Given the current precipitation and a biome's ideal precipitation, return it's weight increase.
*/
float ClairvoyantBiomeArchitect::GetBiomePrecipitationWeightIncrease(const float currentPrecipitation, const float idealPrecipitation) NOEXCEPT
{
	return 1.0f - CatalystBaseMath::Absolute(currentPrecipitation - idealPrecipitation);
}

/*
*	Given the current temperature and a biome's ideal temperature, return it's weight increase.
*/
float ClairvoyantBiomeArchitect::GetBiomeTemperatureWeightIncrease(const float currentTemperature, const float idealTemperature) NOEXCEPT
{
	return 1.0f - CatalystBaseMath::Absolute(currentTemperature - idealTemperature);
}