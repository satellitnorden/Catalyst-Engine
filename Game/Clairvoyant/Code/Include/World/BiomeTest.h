#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/SimplexNoise.h>
#include <Math/Vector3.h>

class BiomeTest final
{

public:

	/*
	*	Returns the biome height.
	*/
	static float Height(const Vector3<float> &position) NOEXCEPT
	{
		//Retrieve the terrain height.
		const float terrainHeight{ TerrainHeight(position) };

		//Retrieve the test height.
		const float testHeight{ TestHeight() };

		//Calculate the test height weight.
		const float testHeightWeight{ CatalystBaseMath::SmoothStep<2>(1.0f - CatalystBaseMath::Minimum<float>(Vector3<float>::LengthSquaredXZ(position) / (1'000.0f * 1'000.0f), 1.0f)) };

		return CatalystBaseMath::LinearlyInterpolate(terrainHeight, testHeight, testHeightWeight);
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
	static float RandomOffset() NOEXCEPT
	{
		constexpr float MAXIMUM_OFFSET{ 1'000.0f };

		static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

		return randomOffset;
	}

	/*
	*	Returns the terrain height.
	*/
	static float TerrainHeight(const Vector3<float> &position) NOEXCEPT
	{
		//Calculate the coordinates.
		const float X{ position._X / 100'000.0f };
		const float Y{ position._Z / 100'000.0f };

		//Calculate the height.
		return SimplexNoise::GenerateIQFBM(Vector2<float>(X, Y), 15, 2.0f, 0.5f, 0.0f);
	}

	/*
	*	Returns the test height.
	*/ 
	static float TestHeight() NOEXCEPT
	{
		static const float height{ CatalystBaseMath::Maximum<float>(TerrainHeight(Vector3<float>(0.0f, 0.0f, 0.0f)), 0.01f) };

		return height;
	}
};