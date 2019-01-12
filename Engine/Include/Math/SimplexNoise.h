#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>

//Simplex noise constants.
namespace SimplexNoiseConstants
{
	constexpr float F2{ 0.366025403f };
	constexpr float G2{ 0.211324865f };

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
	*	Generates a value in the range [-1.0f, 1.0f] at the given coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float Generate(const Vector2<float> &coordinate, const float seed = 0.0f) NOEXCEPT
	{
		Vector2<float> seededCoordinate{ coordinate + seed };

		float n0, n1, n2;

		float s = (seededCoordinate._X + seededCoordinate._Y) * SimplexNoiseConstants::F2; // Hairy factor for 2D
		float xs = seededCoordinate._X + s;
		float ys = seededCoordinate._Y + s;
		int32 i = CatalystBaseMath::Floor<int32>(xs);
		int32 j = CatalystBaseMath::Floor<int32>(ys);

		float t = static_cast<float>(i + j) * SimplexNoiseConstants::G2;
		float X0 = i - t;
		float Y0 = j - t;
		float x0 = seededCoordinate._X - X0;
		float y0 = seededCoordinate._Y - Y0;

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

		float x1 = x0 - i1 + SimplexNoiseConstants::G2;
		float y1 = y0 - j1 + SimplexNoiseConstants::G2;
		float x2 = x0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;
		float y2 = y0 - 1.0f + 2.0f * SimplexNoiseConstants::G2;

		int32 ii = i & 0xff;
		int32 jj = j & 0xff;

		float t0 = 0.5f - x0 * x0 - y0 * y0;

		if (t0 < 0.0f)
		{
			n0 = 0.0f;
		}

		else
		{
			t0 *= t0;
			n0 = t0 * t0 * Gradient(SimplexNoiseConstants::PERMUTATIONS[ii + SimplexNoiseConstants::PERMUTATIONS[jj]], x0, y0);
		}

		float t1 = 0.5f - x1 * x1 - y1 * y1;
		if (t1 < 0.0f)
		{
			n1 = 0.0f;
		}

		else
		{
			t1 *= t1;
			n1 = t1 * t1 * Gradient(SimplexNoiseConstants::PERMUTATIONS[ii + i1 + SimplexNoiseConstants::PERMUTATIONS[jj + j1]], x1, y1);
		}

		float t2 = 0.5f - x2 * x2 - y2 * y2;

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
	*	Generates a value in the range [0.0f, 1.0f] at the given coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float GenerateNormalized(const Vector2<float> &coordinate, const float seed = 0.0f) NOEXCEPT
	{
		return (Generate(coordinate, seed) + 1.0f) * 0.5f;
	}

	/*
	*	Generates a value with derivaties in the range [-1.0f, 1.0f] at the given coordinates.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> GenerateDerivaties(const Vector2<float> &coordinate, const float seed = 0.0f) NOEXCEPT
	{
		//Define constants.,
		constexpr float F2{ 0.366025403f };
		constexpr float G2{ 0.211324865f };

		Vector2<float> seededCoordinate{ coordinate + seed };

		float n0, n1, n2;

		float s = (seededCoordinate._X + seededCoordinate._Y) * F2;
		float xs = seededCoordinate._X + s;
		float ys = seededCoordinate._Y + s;
		int32 i = CatalystBaseMath::Floor<int32>(xs);
		int32 j = CatalystBaseMath::Floor<int32>(ys);

		float t = static_cast<float>(i + j) * G2;
		float X0 = i - t;
		float Y0 = j - t;
		float x0 = seededCoordinate._X - X0;
		float y0 = seededCoordinate._Y - Y0;

		int32 i1, j1;
		if (x0 > y0) { i1 = 1; j1 = 0; }
		else { i1 = 0; j1 = 1; }

		float x1 = x0 - i1 + G2;
		float y1 = y0 - j1 + G2;
		float x2 = x0 - 1.0f + 2.0f * G2;
		float y2 = y0 - 1.0f + 2.0f * G2;

		int32 ii = i & 0xff;
		int32 jj = j & 0xff;

		float gx0, gy0, gx1, gy1, gx2, gy2;

		float t0 = 0.5f - x0 * x0 - y0 * y0;
		float t20, t40;
		if (t0 < 0.0f) t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f;

		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + SimplexNoiseConstants::PERMUTATIONS[jj]], &gx0, &gy0);
			t20 = t0 * t0;
			t40 = t20 * t20;
			n0 = t40 * (gx0 * x0 + gy0 * y0);
		}

		float t1 = 0.5f - x1 * x1 - y1 * y1;
		float t21, t41;
		if (t1 < 0.0f) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f;

		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + i1 + SimplexNoiseConstants::PERMUTATIONS[jj + j1]], &gx1, &gy1);
			t21 = t1 * t1;
			t41 = t21 * t21;
			n1 = t41 * (gx1 * x1 + gy1 * y1);
		}

		float t2 = 0.5f - x2 * x2 - y2 * y2;
		float t22, t42;
		if (t2 < 0.0f) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f;
		else
		{
			Gradient2(SimplexNoiseConstants::PERMUTATIONS[ii + 1 + SimplexNoiseConstants::PERMUTATIONS[jj + 1]], &gx2, &gy2);
			t22 = t2 * t2;
			t42 = t22 * t22;
			n2 = t42 * (gx2 * x2 + gy2 * y2);
		}

		float temp0 = t20 * t0 * (gx0* x0 + gy0 * y0);
		float dnoise_dx = temp0 * x0;
		float dnoise_dy = temp0 * y0;
		float temp1 = t21 * t1 * (gx1 * x1 + gy1 * y1);
		dnoise_dx += temp1 * x1;
		dnoise_dy += temp1 * y1;
		float temp2 = t22 * t2 * (gx2* x2 + gy2 * y2);
		dnoise_dx += temp2 * x2;
		dnoise_dy += temp2 * y2;
		dnoise_dx *= -8.0f;
		dnoise_dy *= -8.0f;
		dnoise_dx += t40 * gx0 + t41 * gx1 + t42 * gx2;
		dnoise_dy += t40 * gy0 + t41 * gy1 + t42 * gy2;
		dnoise_dx *= 40.0f;
		dnoise_dy *= 40.0f;

		return Vector3<float>(70.175438596f * (n0 + n1 + n2), dnoise_dx, dnoise_dy);
	}

	/*
	*	Generates an IQ FBM value in the range [-1.0f, 1.0f] at the given coordinates.
	*/
	FORCE_INLINE static NO_DISCARD float GenerateIQFBM(const Vector2<float> &coordinates, const uint8 octaves, const float lacunarity, const float gain, const float seed = 0.0f) NOEXCEPT
	{
		float sum = 0.0f;
		float amp = 0.5f;
		float dx = 0.0f;
		float dy = 0.0f;
		float freq = 1.0f;

		for (uint8 i{ 0 }; i < octaves; ++i)
		{
			Vector3<float> noise{ GenerateDerivaties(coordinates * freq, seed) };
			dx += noise._Y;
			dy += noise._Z;
			sum += amp * noise._X / (1.0f + dx * dx + dy * dy);
			freq *= lacunarity;
			amp *= gain;
		}

		return sum;
	}

private:

	/*
	*	The gradient function.
	*/
	FORCE_INLINE static float Gradient(const int32 hash, const float x, const float y) NOEXCEPT
	{
		int h = hash & 7;
		float u = h < 4 ? x : y;
		float v = h < 4 ? y : x;

		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
	}

	/*
	*	The gradient 2 function.
	*/
	FORCE_INLINE static void Gradient2(const int32 hash, float *const RESTRICT gx, float *const RESTRICT gy) NOEXCEPT
	{
		//Define constants.
		constexpr float grad2lut[8][2]
		{
			{ -1.0f, -1.0f }, { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
			{ -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
		};

		int32 h = hash & 7;
		*gx = grad2lut[h][0];
		*gy = grad2lut[h][1];
	}
};