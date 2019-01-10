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
	*	Returns the biome material.
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

	/*
	*	Returns the height weight increase.
	*/
	FORCE_INLINE static NO_DISCARD float HeightWeightIncrease(const float height) NOEXCEPT
	{
		return height < 10'000.0f ? 1.0f : 0.0f;
	}

};