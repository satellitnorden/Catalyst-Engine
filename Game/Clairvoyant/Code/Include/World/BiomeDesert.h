#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>
#include <Math/Vector3.h>

class BiomeDesert final
{

public:

	/*
	*	Returns the desert biome weight.
	*/
	static float Weight(const Vector3 &position) NOEXCEPT
	{
		//Calculate the coordinates.
		const float coordinateX{ position._X / 100'000.0f };
		const float coordinateY{ position._Z / 100'000.0f };

		//Calculate the weight.
		return CatalystBaseMath::SmoothStep<1>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(0)));
	}

	/*
	*	Returns the desert biome height.
	*/
	static float Height(const Vector3 &position) NOEXCEPT
	{
		//Calculate the coordinates.
		const float coordinateX{ position._X / 100'000.0f };
		const float coordinateY{ position._Z / 100'000.0f };

		//Calculate the height.
		float height{ -0.825f };

		height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX * 2.0f, coordinateY * 2.0f, GetRandomOffset(1));
		height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX * 4.0f, coordinateY * 4.0f, GetRandomOffset(2)) * 0.5f;
		height += PerlinNoiseGenerator::GenerateRidgedNoise(coordinateX * 8.0f, coordinateY * 8.0f, GetRandomOffset(3)) * 0.25f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 16.0f, coordinateY * 16.0f, GetRandomOffset(4)) * 0.125f;
		height += PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 32.0f, coordinateY * 32.0f, GetRandomOffset(5)) * 0.0625f;

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
		}

		ASSERT(false, "A case should be added here. ):");

		return 0.0f;
	}

};