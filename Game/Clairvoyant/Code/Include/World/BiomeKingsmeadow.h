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

class BiomeKingsmeadow final
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

private:

	/*
	*	Returns a random seed.
	*/
	static float RandomSeed(const uint8 index) NOEXCEPT
	{
		switch (index)
		{
			case 0:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 1:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 2:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}
		}

		ASSERT(false, "Invalid case!");

		return 0.0f;
	}

};