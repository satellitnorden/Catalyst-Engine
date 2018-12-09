#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoise.h>
#include <Math/Vector3.h>

class BiomeDesert final
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
		float height{ 0.0f };

		height += PerlinNoise::Generate(coordinateX * 4.0f, coordinateY * 4.0f, GetRandomOffset(0)) * 0.125f;
		height += PerlinNoise::GenerateRidged(coordinateX * 64.0f, coordinateY * 64.0f, GetRandomOffset(1)) * 0.0625f;
		height += PerlinNoise::GenerateNormalized(coordinateX * 128.0f, coordinateY * 128.0f, GetRandomOffset(2)) * 0.03125f;
		height += PerlinNoise::GenerateNormalized(coordinateX * 256.0f, coordinateY * 256.0f, GetRandomOffset(3)) * 0.015625f;
		height += PerlinNoise::GenerateNormalized(coordinateX * 512.0f, coordinateY * 512.0f, GetRandomOffset(4)) * 0.0078125f;

		return height;
	}

	/*
	*	Returns the biome material.
	*/
	uint8 Material(const Vector3<float> &position) NOEXCEPT
	{

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
		}

		ASSERT(false, "A case should be added here. ):");

		return 0.0f;
	}

};