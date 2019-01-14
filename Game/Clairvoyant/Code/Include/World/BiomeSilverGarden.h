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
			constexpr float NOISE_SCALE_1{ 1'000.0f };
			constexpr float NOISE_SCALE_2{ 100.0f };
			constexpr float NOISE_SCALE_3{ 10.0f };

			//Calculate the coordinates.
			Vector2<float> coordinate1{ position._X / NOISE_SCALE_1, position._Z / NOISE_SCALE_1 };
			Vector2<float> coordinate2{ position._X / NOISE_SCALE_2, position._Z / NOISE_SCALE_2 };
			Vector2<float> coordinate3{ position._X / NOISE_SCALE_3, position._Z / NOISE_SCALE_3 };

			//Calculate the properties.
			const float property1{ SimplexNoise::GenerateNormalized(coordinate1, RandomSeed(0)) };
			const float property2{ SimplexNoise::GenerateNormalized(coordinate1, RandomSeed(1)) };

			//Pick the most fitting material.
			constexpr StaticArray<Quadruple<ClairvoyantTerrainMaterial, float, float, float>, 4> potentialMaterials
			{
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Grass_1, 1.0f, 1.0f, 1.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Grass_2, 0.05f, 0.0f, 1.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Leaves_1, -0.75f, 1.0f, 0.0f),
				Quadruple<ClairvoyantTerrainMaterial, float, float, float>(ClairvoyantTerrainMaterial::Roots_1, -0.85f, 0.0f, 0.0f)
			};

			float bestWeight{ -FLOAT_MAXIMUM };
			ClairvoyantTerrainMaterial bestMaterial{ potentialMaterials[0]._First };

			for (uint8 i{ 0 }; i < potentialMaterials.Size(); ++i)
			{
				const float potentialMaterialWeight{	potentialMaterials[i]._Second
														+ SimplexNoise::GenerateNormalized(coordinate2, RandomSeed((i * 2) + 2))
														+ SimplexNoise::GenerateNormalized(coordinate3, RandomSeed((i * 2) + 1 + 2))
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
		}

		ASSERT(false, "Invalid case!");

		return 0.0f;
	}

};