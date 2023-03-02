#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>

template <uint64 OFFSETS_SIZE_2D>
class SimplexNoiseGenerator final
{

public:

	/*
	*	Initializes this simplex noise generator.
	*/
	FORCE_INLINE SimplexNoiseGenerator() NOEXCEPT
	{
		//Initialize the randomized 2D offsets.
		Memory::Set(&_RandomizedOffsets2D, 0, sizeof(Vector2<float32>) * _RandomizedOffsets2D.Size());
	}

	/*
	*	Shuffles the internal state a bit to produce different results.
	*/
	FORCE_INLINE void Randomize(RandomNumberGenerator *const RESTRICT random_number_generator) NOEXCEPT
	{
		//Randomize the 2D offsets.
		for (Vector2<float32> &randomized_offset_2D : _RandomizedOffsets2D)
		{
			randomized_offset_2D._X = random_number_generator->RandomFloat<float32>();
			randomized_offset_2D._Y = random_number_generator->RandomFloat<float32>();
		}
	}

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the given 2D coordinates.
	*/
	FORCE_INLINE NO_DISCARD float32 Generate2D(const Vector2<float32> &coordinate, const uint8 offset_index = 0) const NOEXCEPT
	{
		const Vector2<float32> offset_coordinate{ coordinate + _RandomizedOffsets2D[offset_index] };

		float32 n0, n1, n2;

		float32 s = (offset_coordinate._X + offset_coordinate._Y) * F2;
		float32 xs = offset_coordinate._X + s;
		float32 ys = offset_coordinate._Y + s;
		int32 i = CatalystBaseMath::Floor<int32>(xs);
		int32 j = CatalystBaseMath::Floor<int32>(ys);

		float32 t = static_cast<float>(i + j) * G2;
		float32 X0 = i - t;
		float32 Y0 = j - t;
		float32 x0 = offset_coordinate._X - X0;
		float32 y0 = offset_coordinate._Y - Y0;

		int32 i1, j1;

		if (x0 > y0)
		{
			i1 = 1;
			j1 = 0;
		}

		else
		{
			i1 = 0;
			j1 = 1;
		}

		float32 x1 = x0 - i1 + G2;
		float32 y1 = y0 - j1 + G2;
		float32 x2 = x0 - 1.0f + 2.0f * G2;
		float32 y2 = y0 - 1.0f + 2.0f * G2;

		int32 ii = i & 0xff;
		int32 jj = j & 0xff;

		float32 t0 = 0.5f - x0 * x0 - y0 * y0;

		if (t0 < 0.0f)
		{
			n0 = 0.0f;
		}

		else
		{
			t0 *= t0;
			n0 = t0 * t0 * Gradient(PERMUTATIONS[ii + PERMUTATIONS[jj]], x0, y0);
		}

		float32 t1 = 0.5f - x1 * x1 - y1 * y1;
		if (t1 < 0.0f)
		{
			n1 = 0.0f;
		}

		else
		{
			t1 *= t1;
			n1 = t1 * t1 * Gradient(PERMUTATIONS[ii + i1 + PERMUTATIONS[jj + j1]], x1, y1);
		}

		float32 t2 = 0.5f - x2 * x2 - y2 * y2;

		if (t2 < 0.0f)
		{
			n2 = 0.0f;
		}

		else
		{
			t2 *= t2;
			n2 = t2 * t2 * Gradient(PERMUTATIONS[ii + 1 + PERMUTATIONS[jj + 1]], x2, y2);
		}

		return 40.0f * (n0 + n1 + n2);
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the given 2D coordinates.
	*/
	FORCE_INLINE NO_DISCARD float32 GenerateNormalized2D(const Vector2<float32> &coordinate, const uint8 offset_index = 0) const NOEXCEPT
	{
		return Generate2D(coordinate, offset_index) * 0.5f + 0.5f;
	}

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the 2D given coordinates, with octaves.
	*/
	FORCE_INLINE NO_DISCARD float32 GenerateOctaves2D(const Vector2<float32> &coordinate, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f) const NOEXCEPT
	{
		float32 noise{ 0.0f };
		float32 total{ 0.0f };
		float32 frequency{ 1.0f };
		float32 amplitude{ 1.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			float32 octave{ Generate2D(coordinate * frequency, i) };

			noise += octave * amplitude;
			total += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return noise / total;
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the 2D given coordinates, with octaves.
	*/
	FORCE_INLINE NO_DISCARD float32 GenerateNormalizedOctaves2D(const Vector2<float32> &coordinate, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f) const NOEXCEPT
	{
		return GenerateOctaves2D(coordinate, octaves, lacunarity, gain) * 0.5f + 0.5f;
	}

private:

	//Constants.
	#include "SimplexNoisePermutations.inl"
	constexpr static float32 F2{ 0.366025403f };
	constexpr static float32 G2{ 0.211324865f };
	constexpr static float32 F3{ 1.0f / 3.0f };
	constexpr static float32 G3{ 1.0f / 6.0f };

	//The randomized 2D offsets.
	StaticArray<Vector2<float32>, OFFSETS_SIZE_2D> _RandomizedOffsets2D;

	/*
	*	The gradient function.
	*/
	FORCE_INLINE float32 Gradient(const int32 hash, const float32 x, const float32 y) const NOEXCEPT
	{
		const int32 h{ hash & 7 };
		const float32 u{ h < 4 ? x : y };
		const float32 v{ h < 4 ? y : x };

		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
	}

};