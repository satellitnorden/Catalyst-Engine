#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint2.h>
#include <Math/Vector3.h>

//World.
#include <World/ClairvoyantWorldConstants.h>

class ClairvoyantBiomeArchitect final
{

public:

	/*
	*	Returns the biome weight at the given position.
	*/
	static float GetBiomeWeightAtPosition(const ClairvoyantBiome biome, const Vector3 &position) NOEXCEPT;

	/*
	*	Returns the biome height at the given position.
	*/
	static float GetBiomeHeightAtPosition(const ClairvoyantBiome biome, const Vector3 &position) NOEXCEPT;

};