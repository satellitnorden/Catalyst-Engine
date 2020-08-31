#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

//Simplex noise constants.
namespace SimplexNoiseConstants
{
	constexpr float32 F2{ 0.366025403f };
	constexpr float32 G2{ 0.211324865f };
	constexpr float32 F3{ 1.0f / 3.0f };
	constexpr float32 G3{ 1.0f / 6.0f };

	constexpr uint8 PERMUTATIONS[512] 
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
}

class SimplexNoise final
{

public:

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the given 2D coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float32 Generate2D(const Vector2<float32> &coordinate, const Vector2<float32>& offset) NOEXCEPT
	{
		Vector2<float32> offset_coordinate{ coordinate + offset };

		float32 n0, n1, n2;

		float32 s = (offset_coordinate._X + offset_coordinate._Y) * SimplexNoiseConstants::F2;
		float32 xs = offset_coordinate._X + s;
		float32 ys = offset_coordinate._Y + s;
		int32 i = CatalystBaseMath::Floor<int32>(xs);
		int32 j = CatalystBaseMath::Floor<int32>(ys);

		float32 t = static_cast<float>(i + j) * SimplexNoiseConstants::G2;
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

		float32 x1 = x0 - i1 + SimplexNoiseConstants::G2;
		float32 y1 = y0 - j1 + SimplexNoiseConstants::G2;
		float32 x2 = x0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;
		float32 y2 = y0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;

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
			n0 = t0 * t0 * Gradient(SimplexNoiseConstants::PERMUTATIONS[ii + SimplexNoiseConstants::PERMUTATIONS[jj]], x0, y0);
		}

		float32 t1 = 0.5f - x1 * x1 - y1 * y1;
		if (t1 < 0.0f)
		{
			n1 = 0.0f;
		}

		else
		{
			t1 *= t1;
			n1 = t1 * t1 * Gradient(SimplexNoiseConstants::PERMUTATIONS[ii + i1 + SimplexNoiseConstants::PERMUTATIONS[jj + j1]], x1, y1);
		}

		float32 t2 = 0.5f - x2 * x2 - y2 * y2;

		if (t2 < 0.0f)
		{
			n2 = 0.0f;
		}

		else
		{
			t2 *= t2;
			n2 = t2 * t2 * Gradient(SimplexNoiseConstants::PERMUTATIONS[ii + 1 + SimplexNoiseConstants::PERMUTATIONS[jj + 1]], x2, y2);
		}

		return 40.0f * (n0 + n1 + n2);
	}

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the given 2D coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float32 Generate3D(const Vector3<float32> &coordinate, const Vector3<float32>& offset) NOEXCEPT
	{
		const Vector3<float32> offset_coordinate{ coordinate + offset };

		float32 n0, n1, n2, n3;

		float32 s = (offset_coordinate._X + offset_coordinate._Y + offset_coordinate._Z) * SimplexNoiseConstants::F3;
		int32 i = CatalystBaseMath::Floor<int32>(offset_coordinate._X + s);
		int32 j = CatalystBaseMath::Floor<int32>(offset_coordinate._Y + s);
		int32 k = CatalystBaseMath::Floor<int32>(offset_coordinate._Z + s);
		float32 t = (i + j + k) * SimplexNoiseConstants::G3;
		float32 X0 = i - t;
		float32 Y0 = j - t;
		float32 Z0 = k - t;
		float32 x0 = offset_coordinate._X - X0;
		float32 y0 = offset_coordinate._Y - Y0;
		float32 z0 = offset_coordinate._Z - Z0;

		int32 i1, j1, k1;
		int32 i2, j2, k2;

		if (x0 >= y0)
		{
			if (y0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
			}
			
			else if (x0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
			}

			else
			{
				i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
			}
		}

		else
		{
			if (y0 < z0)
			{
				i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
			}
			
			else if (x0 < z0) 
			{
				i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
			}
			
			else
			{
				i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
			}
		}

		float32 x1 = x0 - i1 + SimplexNoiseConstants::G3;
		float32 y1 = y0 - j1 + SimplexNoiseConstants::G3;
		float32 z1 = z0 - k1 + SimplexNoiseConstants::G3;
		float32 x2 = x0 - i2 + 2.0f * SimplexNoiseConstants::G3;
		float32 y2 = y0 - j2 + 2.0f * SimplexNoiseConstants::G3;
		float32 z2 = z0 - k2 + 2.0f * SimplexNoiseConstants::G3;
		float32 x3 = x0 - 1.0f + 3.0f * SimplexNoiseConstants::G3;
		float32 y3 = y0 - 1.0f + 3.0f * SimplexNoiseConstants::G3;
		float32 z3 = z0 - 1.0f + 3.0f * SimplexNoiseConstants::G3;

		int32 gi0 = SimplexNoiseConstants::PERMUTATIONS[i + SimplexNoiseConstants::PERMUTATIONS[j + SimplexNoiseConstants::PERMUTATIONS[k]]];
		int32 gi1 = SimplexNoiseConstants::PERMUTATIONS[i + i1 + SimplexNoiseConstants::PERMUTATIONS[j + j1 + SimplexNoiseConstants::PERMUTATIONS[k + k1]]];
		int32 gi2 = SimplexNoiseConstants::PERMUTATIONS[i + i2 + SimplexNoiseConstants::PERMUTATIONS[j + j2 + SimplexNoiseConstants::PERMUTATIONS[k + k2]]];
		int32 gi3 = SimplexNoiseConstants::PERMUTATIONS[i + 1 + SimplexNoiseConstants::PERMUTATIONS[j + 1 + SimplexNoiseConstants::PERMUTATIONS[k + 1]]];

		float32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;

		if (t0 < 0)
		{
			n0 = 0.0;
		}
		
		else
		{
			t0 *= t0;
			n0 = t0 * t0 * Gradient(gi0, x0, y0, z0);
		}

		float32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;

		if (t1 < 0)
		{
			n1 = 0.0;
		}
		
		else
		{
			t1 *= t1;
			n1 = t1 * t1 * Gradient(gi1, x1, y1, z1);
		}

		float32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;

		if (t2 < 0)
		{
			n2 = 0.0;
		}
		
		else
		{
			t2 *= t2;
			n2 = t2 * t2 * Gradient(gi2, x2, y2, z2);
		}

		float32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;

		if (t3 < 0)
		{
			n3 = 0.0;
		}
		
		else
		{
			t3 *= t3;
			n3 = t3 * t3 * Gradient(gi3, x3, y3, z3);
		}

		return 32.0f * (n0 + n1 + n2 + n3);
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the given 2D coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float32 GenerateNormalized2D(const Vector2<float32> &coordinate, const Vector2<float32>& offset) NOEXCEPT
	{
		return Generate2D(coordinate, offset) * 0.5f + 0.5f;
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the given 3D coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float32 GenerateNormalized3D(const Vector3<float32> &coordinate, const Vector3<float32>& offset) NOEXCEPT
	{
		return Generate3D(coordinate, offset) * 0.5f + 0.5f;
	}

	/*
	*	Generates a value with derivatives in the range [-1.0f, 1.0f] at the 2D given coordinates.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> GenerateDerivatives2D(const Vector2<float> &coordinate, const Vector2<float>& offset) NOEXCEPT
	{
		Vector2<float32> offset_coordinate{ coordinate + offset };

		float32 n0, n1, n2;

		float32 s = (offset_coordinate._X + offset_coordinate._Y) * SimplexNoiseConstants::F2;
		float32 xs = offset_coordinate._X + s;
		float32 ys = offset_coordinate._Y + s;
		int32 i = CatalystBaseMath::Floor<int32>(xs);
		int32 j = CatalystBaseMath::Floor<int32>(ys);

		float32 t = static_cast<float32>(i + j) * SimplexNoiseConstants::G2;
		float32 X0 = i - t;
		float32 Y0 = j - t;
		float32 x0 = offset_coordinate._X - X0;
		float32 y0 = offset_coordinate._Y - Y0;

		int32 i1, j1;
		if (x0 > y0) { i1 = 1; j1 = 0; }
		else { i1 = 0; j1 = 1; }

		float32 x1 = x0 - i1 + SimplexNoiseConstants::G2;
		float32 y1 = y0 - j1 + SimplexNoiseConstants::G2;
		float32 x2 = x0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;
		float32 y2 = y0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;

		int32 ii = i & 0xff;
		int32 jj = j & 0xff;

		float32 gx0, gy0, gx1, gy1, gx2, gy2;

		float32 t0 = 0.5f - x0 * x0 - y0 * y0;
		float32 t20, t40;
		if (t0 < 0.0f) t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f;

		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + SimplexNoiseConstants::PERMUTATIONS[jj]], &gx0, &gy0);
			t20 = t0 * t0;
			t40 = t20 * t20;
			n0 = t40 * (gx0 * x0 + gy0 * y0);
		}

		float32 t1 = 0.5f - x1 * x1 - y1 * y1;
		float32 t21, t41;
		if (t1 < 0.0f) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f;

		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + i1 + SimplexNoiseConstants::PERMUTATIONS[jj + j1]], &gx1, &gy1);
			t21 = t1 * t1;
			t41 = t21 * t21;
			n1 = t41 * (gx1 * x1 + gy1 * y1);
		}

		float32 t2 = 0.5f - x2 * x2 - y2 * y2;
		float32 t22, t42;
		if (t2 < 0.0f) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f;
		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + 1 + SimplexNoiseConstants::PERMUTATIONS[jj + 1]], &gx2, &gy2);
			t22 = t2 * t2;
			t42 = t22 * t22;
			n2 = t42 * (gx2 * x2 + gy2 * y2);
		}

		float32 temp0 = t20 * t0 * (gx0* x0 + gy0 * y0);
		float32 dnoise_dx = temp0 * x0;
		float32 dnoise_dy = temp0 * y0;
		float32 temp1 = t21 * t1 * (gx1 * x1 + gy1 * y1);
		dnoise_dx += temp1 * x1;
		dnoise_dy += temp1 * y1;
		float32 temp2 = t22 * t2 * (gx2* x2 + gy2 * y2);
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
	FORCE_INLINE static NO_DISCARD float32 GenerateDerivativesOctaved2D(const Vector2<float32> &coordinates, const ArrayProxy<Vector2<float32>> &offsets, const uint8 octaves, const float32 lacunarity, const float32 gain, const float32 derivative_weight, const float32 seed = 0.0f) NOEXCEPT
	{
		float32 total{ 0.0f };
		float32 maximum{ 0.0f };
		float32 amplitude{  1.0f };
		float32 frequency{ 1.0f };
		float32 derivative_x{ 0.0f };
		float32 derivative_y{ 0.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			Vector3<float32> noise{ GenerateDerivatives2D(coordinates * frequency, offsets.Size() > i ? offsets[i] : Vector2<float32>(0.0f, 0.0f)) };

			derivative_x += noise._Y;
			derivative_y += noise._Z;

			total += noise._X / CatalystBaseMath::LinearlyInterpolate(1.0f, 1.0f + derivative_x * derivative_x + derivative_y * derivative_y, derivative_weight) * amplitude;

			maximum += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return total / maximum;
	}

	/*
	*	Generates a value with derivatives in the range [0.0f, 1.0f] at the given 2D coordinates, with octaves.
	*/
	FORCE_INLINE static NO_DISCARD float32 GenerateNormalizedDerivativesOctaved2D(const Vector2<float32> &coordinates, const ArrayProxy<Vector2<float32>> &offsets, const uint8 octaves, const float32 lacunarity, const float32 gain, const float32 derivative_weight, const float32 seed = 0.0f) NOEXCEPT
	{
		return GenerateDerivativesOctaved2D(coordinates, offsets, octaves, lacunarity, gain, derivative_weight, seed) * 0.5f + 0.5f;
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the 2D given coordinates, with octaves.
	*/
	FORCE_INLINE static NO_DISCARD float32 GenerateNormalizedOctaves2D(const Vector2<float32>& coordinate, const ArrayProxy<Vector2<float32>>& offsets, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f) NOEXCEPT
	{
		float32 noise{ 0.0f };
		float32 total{ 0.0f };
		float32 frequency{ 1.0f };
		float32 amplitude{ 1.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			float32 octave{ GenerateNormalized2D(coordinate * frequency, offsets.Size() > i ? offsets[i] : Vector2<float32>(0.0f, 0.0f)) };

			noise += octave * amplitude;
			total += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return noise / total;
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the 3D given coordinates, with octaves.
	*/
	FORCE_INLINE static NO_DISCARD float32 GenerateNormalizedOctaves3D(const Vector3<float32> &coordinate, const ArrayProxy<Vector3<float32>> &offsets, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f) NOEXCEPT
	{
		float32 noise{ 0.0f };
		float32 total{ 0.0f };
		float32 frequency{ 1.0f };
		float32 amplitude{ 1.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			float32 octave{ GenerateNormalized3D(coordinate * frequency, offsets.Size() > i ? offsets[i] : Vector3<float32>(0.0f, 0.0f, 0.0f)) };

			noise += octave * amplitude;
			total += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return noise / total;
	}

	/*
	*	Generates a ridged value in the range [0.0f, 1.0f] at the 2D given coordinates, with octaves
	*/
	FORCE_INLINE static NO_DISCARD float GenerateRidgedOctaves2D(const Vector2<float32>& coordinate, const ArrayProxy<Vector2<float32>>& offsets, const uint8 octaves, const float32 lacunarity = 2.0f, const float32 gain = 0.5f) NOEXCEPT
	{
		float32 noise{ 0.0f };
		float32 total{ 0.0f };
		float32 frequency{ 1.0f };
		float32 amplitude{ 1.0f };

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			float octave{ 1.0f - CatalystBaseMath::Absolute<float32>(Generate2D(coordinate * frequency, offsets.Size() > i ? offsets[i] : Vector2<float32>(0.0f, 0.0f))) };

			noise += octave * amplitude;
			total += amplitude;

			frequency *= lacunarity;
			amplitude *= gain;
		}

		return noise / total;
	}

private:

	/*
	*	The gradient function.
	*/
	FORCE_INLINE static float32 Gradient(const int32 hash, const float32 x, const float32 y) NOEXCEPT
	{
		const int32 h{ hash & 7 };
		const float32 u{ h < 4 ? x : y };
		const float32 v{ h < 4 ? y : x };

		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
	}

	/*
	*	The gradient function.
	*/
	FORCE_INLINE static float32 Gradient(const int32 hash, const float32 x, const float32 y, const float32 z) NOEXCEPT
	{
		const int32 h{ hash & 15 };
		const float32 u{ h < 8 ? x : y };
		const float32 v{ h < 4 ? y : h == 12 || h == 14 ? x : z };

		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}

	/*
	*	The gradient 2 function.
	*/
	FORCE_INLINE static void Gradient2(const int32 hash, float32 *const RESTRICT gx, float32 *const RESTRICT gy) NOEXCEPT
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