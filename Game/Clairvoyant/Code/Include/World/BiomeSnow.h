#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>
#include <Math/Vector3.h>

class BiomeSnow final
{

public:

	/*
	*	Returns the biome height.
	*/
	static float Height(const Vector3<float> &position) NOEXCEPT
	{
		//Calculate the coordinates.
		const float coordinateX{ position._X / 100'000.0f };
		const float coordinateY{ position._Z / 100'000.0f };

		//Calculate the height.
		float height{ 1.0f };

		height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX * 4.0f, coordinateY * 4.0f, GetRandomOffset(0)) * 2.0f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 8.0f, coordinateY * 8.0f, GetRandomOffset(1));
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 16.0f, coordinateY * 16.0f, GetRandomOffset(2)) * 0.5f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 32.0f, coordinateY * 32.0f, GetRandomOffset(3)) * 0.25f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 64.0f, coordinateY * 64.0f, GetRandomOffset(4)) * 0.125f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 128.0f, coordinateY * 128.0f, GetRandomOffset(5)) * 0.065f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 256.0f, coordinateY * 256.0f, GetRandomOffset(6)) * 0.03125f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 512.0f, coordinateY * 512.0f, GetRandomOffset(7)) * 0.015625f;

		return height;
	}

private:

	/*
	*	Returns a random offset.
	*/
	static float GetRandomOffset(const uint8 index) NOEXCEPT
	{
		constexpr float MAXIMUM_OFFSET{ 1'000.0f };

		switch (index)
		{
			case 0:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 1:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 2:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 3:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 4:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 5:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 6:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}

			case 7:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

				return randomOffset;
			}
		}

		ASSERT(false, "A case should be added here. ):");

		return 0.0f;
	}

};