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
		int32 i = BaseMath::Floor<int32>(xs);
		int32 j = BaseMath::Floor<int32>(ys);

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

	/*
	*	Generates a value with derivatives in the range [-1.0f, 1.0f] at the 2D given coordinates.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GenerateDerivatives2D(const Vector2<float32> &coordinate, const uint8 offset_index = 0) const NOEXCEPT
	{
		Vector2<float32> offset_coordinate{ coordinate + _RandomizedOffsets2D[offset_index] };

		float32 n0, n1, n2;

		float32 s = (offset_coordinate._X + offset_coordinate._Y) * F2;
		float32 xs = offset_coordinate._X + s;
		float32 ys = offset_coordinate._Y + s;
		int32 i = BaseMath::Floor<int32>(xs);
		int32 j = BaseMath::Floor<int32>(ys);

		float32 t = static_cast<float32>(i + j) * G2;
		float32 X0 = i - t;
		float32 Y0 = j - t;
		float32 x0 = offset_coordinate._X - X0;
		float32 y0 = offset_coordinate._Y - Y0;

		int32 i1, j1;
		if (x0 > y0) { i1 = 1; j1 = 0; }
		else { i1 = 0; j1 = 1; }

		float32 x1 = x0 - i1 + G2;
		float32 y1 = y0 - j1 + G2;
		float32 x2 = x0 - 1.0f + 2.0f * G2;
		float32 y2 = y0 - 1.0f + 2.0f * G2;

		int32 ii = i & 0xff;
		int32 jj = j & 0xff;

		float32 gx0, gy0, gx1, gy1, gx2, gy2;

		float32 t0 = 0.5f - x0 * x0 - y0 * y0;
		float32 t20, t40;
		if (t0 < 0.0f) t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f;

		else
		{
			Gradient2(PERMUTATIONS[ii + PERMUTATIONS[jj]], &gx0, &gy0);
			t20 = t0 * t0;
			t40 = t20 * t20;
			n0 = t40 * (gx0 * x0 + gy0 * y0);
		}

		float32 t1 = 0.5f - x1 * x1 - y1 * y1;
		float32 t21, t41;
		if (t1 < 0.0f) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f;

		else
		{
			Gradient2(PERMUTATIONS[ii + i1 + PERMUTATIONS[jj + j1]], &gx1, &gy1);
			t21 = t1 * t1;
			t41 = t21 * t21;
			n1 = t41 * (gx1 * x1 + gy1 * y1);
		}

		float32 t2 = 0.5f - x2 * x2 - y2 * y2;
		float32 t22, t42;
		if (t2 < 0.0f) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f;
		else
		{
			Gradient2(PERMUTATIONS[ii + 1 + PERMUTATIONS[jj + 1]], &gx2, &gy2);
			t22 = t2 * t2;
			t42 = t22 * t22;
			n2 = t42 * (gx2 * x2 + gy2 * y2);
		}

		float32 temp0 = t20 * t0 * (gx0 * x0 + gy0 * y0);
		float32 dnoise_dx = temp0 * x0;
		float32 dnoise_dy = temp0 * y0;
		float32 temp1 = t21 * t1 * (gx1 * x1 + gy1 * y1);
		dnoise_dx += temp1 * x1;
		dnoise_dy += temp1 * y1;
		float32 temp2 = t22 * t2 * (gx2 * x2 + gy2 * y2);
		dnoise_dx += temp2 * x2;
		dnoise_dy += temp2 * y2;
		dnoise_dx *= -8.0f;
		dnoise_dy *= -8.0f;
		dnoise_dx += t40 * gx0 + t41 * gx1 + t42 * gx2;
		dnoise_dy += t40 * gy0 + t41 * gy1 + t42 * gy2;
		dnoise_dx *= 40.0f;
		dnoise_dy *= 40.0f;

		return Vector3<float32>(70.175438596f * (n0 + n1 + n2), dnoise_dx, dnoise_dy);
	}

	/*
	*	Generates a value with derivatives in the range [-1.0f, 1.0f] at the given 2D coordinates, with octaves.
	*/
	FORCE_INLINE NO_DISCARD float32 GenerateDerivativesOctaved2D(const Vector2<float32> &coordinate, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f, const float32 derivative_weight = 1.0f) const NOEXCEPT
	{
		float32 total{ 0.0f };
		float32 maximum{ 0.0f };
		float32 amplitude{ 1.0f };
		float32 frequency{ 1.0f };
		float32 derivative_x{ 0.0f };
		float32 derivative_y{ 0.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			Vector3<float32> noise{ GenerateDerivatives2D(coordinate * frequency, i) };

			derivative_x += noise._Y;
			derivative_y += noise._Z;

			total += noise._X / BaseMath::LinearlyInterpolate(1.0f, 1.0f + derivative_x * derivative_x + derivative_y * derivative_y, derivative_weight) * amplitude;

			maximum += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return total / maximum;
	}

	/*
	*	Generates a value with derivatives in the range [0.0f, 1.0f] at the given 2D coordinates, with octaves.
	*/
	FORCE_INLINE NO_DISCARD float32 GenerateNormalizedDerivativesOctaved2D(const Vector2<float32> &coordinate, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f, const float32 derivative_weight = 1.0f) const NOEXCEPT
	{
		return GenerateDerivativesOctaved2D(coordinate, octaves, lacunarity, gain, derivative_weight) * 0.5f + 0.5f;
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

	/*
	*	The gradient 2 function.
	*/
	FORCE_INLINE void Gradient2(const int32 hash, float32* const RESTRICT gx, float32* const RESTRICT gy) const NOEXCEPT
	{
		//Define constants.
		constexpr float32 GRADIENT_LOOKUP[8][2]
		{
			{ -1.0f, -1.0f }, { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
			{ -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
		};

		const int32 h{ hash & 7 };

		*gx = GRADIENT_LOOKUP[h][0];
		*gy = GRADIENT_LOOKUP[h][1];
	}

};