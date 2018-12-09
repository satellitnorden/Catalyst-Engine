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
	*	Returns the biome weights at the given position.
	*/
	static void GetBiomeWeightsAtPosition(const Vector3<float> &position, StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> *const RESTRICT weights) NOEXCEPT;

	/*
	*	Returns the biome height at the given position.
	*/
	static float GetBiomeHeightAtPosition(const ClairvoyantBiome biome, const Vector3<float> &position) NOEXCEPT;

};