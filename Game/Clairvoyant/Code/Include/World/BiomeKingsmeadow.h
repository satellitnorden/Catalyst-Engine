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

class BiomeKingsMeadow final
{

public:

	/*
	*	Returns the ideal precipitation for this biome.
	*/
	FORCE_INLINE static NO_DISCARD float IdealPrecipitation() NOEXCEPT
	{
		return 0.5f;
	}

	/*
	*	Returns the ideal temperature for this biome.
	*/
	FORCE_INLINE static NO_DISCARD float IdealTemperature() NOEXCEPT
	{
		return 0.5f;
	}

	/*
	*	Returns the material for this biome.
	*/
	FORCE_INLINE static NO_DISCARD uint8 Material(const Vector3<float> &position) NOEXCEPT
	{
		Vector3<float> normal;
		TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &normal);

		if (Vector3<float>::DotProduct(normal, Vector3<float>::UP) > 0.5f)
		{
			return UNDERLYING(ClairvoyantTerrainMaterial::Grass_1);
		}

		else
		{
			return UNDERLYING(ClairvoyantTerrainMaterial::Rock_1);
		}
	}

};