#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/SimplexNoise.h>
#include <Math/Vector3.h>

//Systems.
#include <Systems/TerrainSystem.h>

class BiomeWalkerValley final
{

public:

	/*
	*	Returns the ideal precipitation for this biome.
	*/
	FORCE_INLINE static NO_DISCARD float IdealPrecipitation() NOEXCEPT
	{
		return 0.0f;
	}

	/*
	*	Returns the ideal temperature for this biome.
	*/
	FORCE_INLINE static NO_DISCARD float IdealTemperature() NOEXCEPT
	{
		return 1.0f;
	}

	/*
	*	Returns the material for this biome.
	*/
	FORCE_INLINE static NO_DISCARD uint8 Material(const Vector3<float> &position, const float height, const Vector3<float> &normal) NOEXCEPT
	{
		if (Vector3<float>::DotProduct(normal, Vector3<float>::UP) > 0.5f)
		{
			return UNDERLYING(ClairvoyantTerrainMaterial::Sand_1);
		}

		else
		{
			return UNDERLYING(ClairvoyantTerrainMaterial::Cliff_1);
		}
	}

};