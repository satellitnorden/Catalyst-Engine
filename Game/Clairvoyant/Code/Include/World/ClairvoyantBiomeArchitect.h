#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/GridPoint2.h>
#include <Math/Vector3.h>

//World.
#include <World/ClairvoyantWorldConstants.h>

class ClairvoyantBiomeArchitect final
{

public:

	/*
	*	Returns the biome weight for the given biome at the given position.
	*/
	static float GetBiomeWeightAtPosition(const Vector3<float> &position, const ClairvoyantBiome biome) NOEXCEPT;

	/*
	*	Returns the biome height weight increase for the given biome at the given position.
	*/
	static float GetBiomeHeightWeightIncrease(const ClairvoyantBiome biome, const float height) NOEXCEPT;

	/*
	*	Returns the biome weights at the given position.
	*/
	static void GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT;

	/*
	*	Returns the material of the given biome of that given position.
	*/
	static uint8 GetBiomeMaterialAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT;

};