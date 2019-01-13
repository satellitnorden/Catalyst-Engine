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

class BiomeSilverGarden final
{

public:

	/*
	*	Returns the ideal precipitation for this biome.
	*/
	FORCE_INLINE static NO_DISCARD float IdealPrecipitation() NOEXCEPT
	{
		return 1.0f;
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
			/*
			//Define constants.
			constexpr float NOISE_SCALE{ 100.0f };

			//Calculate the coordinate.
			Vector2<float> coordinate{ position._X / NOISE_SCALE, position._Z / NOISE_SCALE };

			//Pick the most fitting material.
			constexpr StaticArray<ClairvoyantTerrainMaterial, 4> potentialMaterials
			{
				ClairvoyantTerrainMaterial::Grass_1,
				ClairvoyantTerrainMaterial::Grass_2,
				ClairvoyantTerrainMaterial::Leaves_1,
				ClairvoyantTerrainMaterial::Roots_1
			};

			float bestWeight{ -FLOAT_MAXIMUM };
			ClairvoyantTerrainMaterial bestMaterial{ potentialMaterials[0] };

			for (uint8 i{ 0 }; i < potentialMaterials.Size(); ++i)
			{
				const float potentialMaterialWeight{ SimplexNoise::GenerateNormalized(coordinate, RandomSeed(i)) };

				if (bestWeight < potentialMaterialWeight)
				{
					bestWeight = potentialMaterialWeight;
					bestMaterial = potentialMaterials[i];
				}
			}

			return static_cast<uint8>(bestMaterial);
			*/

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
	FORCE_INLINE static NO_DISCARD float RandomSeed(const uint8 index) NOEXCEPT
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

			case 3:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}
		}

		ASSERT(false, "Invalid case!");

		return 0.0f;
	}

};