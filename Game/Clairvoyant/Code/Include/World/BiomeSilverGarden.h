#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Quadruple.h>

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
			//Define constants.
			constexpr float GAIN{ 0.8f };
			constexpr float NOISE_SCALE{ 10'000.0f };
			constexpr uint8 OCTAVES{ 10 };

			//Calculate the coordinate.
			Vector2<float> coordinate{ position._X / NOISE_SCALE, position._Z / NOISE_SCALE };

			//Calculate the properties.
			float property1{ 0.0f };
			float property2{ 0.0f };

			{
				float amplitude{ 1.0f };
				float frequency{ 1.0f };
				float total{ 0.0f };

				for (uint8 i{ 0 }; i < OCTAVES; ++i)
				{
					property1 += SimplexNoise::GenerateNormalized(coordinate * frequency, RandomSeed(i)) * amplitude;

					total += amplitude;

					amplitude *= GAIN;
					frequency *= 2.0f;
				}

				property1 /= total;
			}

			{
				float amplitude{ 1.0f };
				float frequency{ 1.0f };
				float total{ 0.0f };

				for (uint8 i{ 0 }; i < OCTAVES; ++i)
				{
					property2 += SimplexNoise::GenerateNormalized(coordinate * frequency, RandomSeed(OCTAVES + i)) * amplitude;

					total += amplitude;

					amplitude *= GAIN;
					frequency *= 2.0f;
				}

				property2 /= total;
			}

			//Pick the most fitting material.
			constexpr StaticArray<Quadruple<ClairvoyantTerrainMaterial, float, float, float>, 4> potentialMaterials
			{
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Grass_1, 0.3f, 1.0f, 1.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Grass_2, -0.1f, 0.0f, 1.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Leaves_1, -0.1f, 1.0f, 0.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Roots_1, -0.45f, 0.0f, 0.0f)
			};

			float bestWeight{ -FLOAT_MAXIMUM };
			ClairvoyantTerrainMaterial bestMaterial{ potentialMaterials[0]._First };

			for (uint8 i{ 0 }; i < potentialMaterials.Size(); ++i)
			{
				const float potentialMaterialWeight{	potentialMaterials[i]._Second
														+ (1.0f - CatalystBaseMath::Absolute(property1 - potentialMaterials[i]._Third))
														+ (1.0f - CatalystBaseMath::Absolute(property2 - potentialMaterials[i]._Fourth)) };

				if (bestWeight < potentialMaterialWeight)
				{
					bestWeight = potentialMaterialWeight;
					bestMaterial = potentialMaterials[i]._First;
				}
			}

			return static_cast<uint8>(bestMaterial);
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

			case 4:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 5:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 6:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 7:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 8:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 9:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 10:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 11:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 12:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 13:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 14:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 15:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 16:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 17:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 18:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}

			case 19:
			{
				static float seed{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return seed;
			}
		}

		ASSERT(false, "Invalid case!");

		return 0.0f;
	}

};