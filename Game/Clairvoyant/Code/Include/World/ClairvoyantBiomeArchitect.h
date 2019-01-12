#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/GridPoint2.h>
#include <Math/Vector3.h>

//World.
#include <World/BiomeProperties.h>
#include <World/ClairvoyantWorldConstants.h>

class ClairvoyantBiomeArchitect final
{

public:

	/*
	*	Returns the biome weights at the given position.
	*/
	static void GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT;

	/*
	*	Returns the material of the given biome of that given position.
	*/
	static uint8 GetBiomeMaterialAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position, const float height, const Vector3<float> &normal) NOEXCEPT;

private:

	/*
	*	Returns the biome properties at the given position.
	*/
	static void GetBiomePropertiesAtPosition(const Vector3<float> &position, BiomeProperties *const RESTRICT properties) NOEXCEPT;

	/*
	*	Returns the biome weight for the given biome with the given biome properties.
	*/
	static float GetBiomeWeightAtPosition(const BiomeProperties &properties, const ClairvoyantBiome biome) NOEXCEPT;

	/*
	*	Returns the ideal precipitation of the given biome.
	*/
	static float GetBiomeIdealPrecipitation(const ClairvoyantBiome biome) NOEXCEPT;

	/*
	*	Returns the ideal temperature of the given biome.
	*/
	static float GetBiomeIdealTemperature(const ClairvoyantBiome biome) NOEXCEPT;

	/*
	*	Given the current precipitation and a biome's ideal precipitation, return it's weight increase.
	*/
	static float GetBiomePrecipitationWeightIncrease(const float currentPrecipitation, const float idealPrecipitation) NOEXCEPT;

	/*
	*	Given the current temperature and a biome's ideal temperature, return it's weight increase.
	*/
	static float GetBiomeTemperatureWeightIncrease(const float currentTemperature, const float idealTemperature) NOEXCEPT;

};