#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>

namespace PerlinNoiseGenerator
{

	//The permutations array.
	uint8 permutations[512] = { 151,160,137,91,90,15,
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

								//First 256 values repeated to avoid buffer overflow.
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

	/*
	*	Generates noise at the given coordinates.
	*/
	static constexpr float GenerateNoise(float X, float Y, float Z, const float randomOffset = 0.0f) CATALYST_NOEXCEPT
	{
		X += randomOffset;
		Y += randomOffset;
		Z += randomOffset;

		uint32 xInt = static_cast<uint32>(X) & 255;
		uint32 yInt = static_cast<uint32>(Y) & 255;
		uint32 zInt = static_cast<uint32>(Z) & 255;

		float xFloat = X - static_cast<uint32>(X);
		float yFloat = Y - static_cast<uint32>(Y);
		float zFloat = Z - static_cast<uint32>(Z);

		float u = Fade(xFloat);
		float v = Fade(yFloat);
		float w = Fade(zFloat);

		uint32 aaa = permutations[permutations[permutations[xInt] + yInt] + zInt];
		uint32 aba = permutations[permutations[permutations[xInt] + yInt + 1] + zInt];
		uint32 aab = permutations[permutations[permutations[xInt] + yInt] + zInt + 1];
		uint32 abb = permutations[permutations[permutations[xInt] + yInt + 1] + zInt + 1];
		uint32 baa = permutations[permutations[permutations[xInt + 1] + yInt] + zInt];
		uint32 bba = permutations[permutations[permutations[xInt + 1] + yInt + 1] + zInt];
		uint32 bab = permutations[permutations[permutations[xInt + 1] + yInt] + zInt + 1];
		uint32 bbb = permutations[permutations[permutations[xInt + 1] + yInt + 1] + zInt + 1];

		float x1 = GameMath::LinearlyInterpolate(Gradient(aaa, xFloat, yFloat, zFloat), Gradient(baa, xFloat - 1.0f, yFloat, zFloat), u);

		float x2 = GameMath::LinearlyInterpolate(Gradient(aba, xFloat, yFloat - 1.0f, zFloat), Gradient(bba, xFloat - 1.0f, yFloat - 1.0f, zFloat), u);

		float y1 = GameMath::LinearlyInterpolate(x1, x2, v);

		x1 = GameMath::LinearlyInterpolate(Gradient(aab, xFloat, yFloat, zFloat - 1.0f), Gradient(bab, xFloat - 1.0f, yFloat, zFloat - 1.0f), u);

		x2 = GameMath::LinearlyInterpolate(Gradient(abb, xFloat, yFloat - 1.0f, zFloat - 1.0f), Gradient(bbb, xFloat - 1.0f, yFloat - 1.0f, zFloat - 1.0f), u);

		float y2 = GameMath::LinearlyInterpolate(x1, x2, v);

		return (GameMath::LinearlyInterpolate(y1, y2, w) + 1.0f) / 2.0f;
	}

	/*
	*	Generates octaved noise.
	*/
	float GenerateOctavedNoise(float X, float Y, float Z, const uint8 octaves, const float persistence) CATALYST_NOEXCEPT
	{
		float total{ 0.0f };
		float frequency{ 1.0f };
		float amplitude{ 1.0f };
		float max{ 0.0f };

		for (uint8 i = 0; i < octaves; ++i)
		{
			total += GenerateNoise(X * frequency, Y * frequency, Z * frequency) * amplitude;

			max += amplitude;

			amplitude *= persistence;
			frequency *= 2.0f;
		}

		return total / max;
	}

	/*
	*	The fade function.
	*/
	static constexpr float Fade(const float value) CATALYST_NOEXCEPT
	{
		return value * value * value * (value * (value * 6.0f - 15.0f) + 10.0f);
	}

	/*
	*	The gradient function.
	*/
	static constexpr float Gradient(uint32 hash, float X, float Y, float Z) CATALYST_NOEXCEPT
	{
		switch (hash & 0xF)
		{
			default: return 0;

			case 0x0: return  X + Y;
			case 0x1: return -X + Y;
			case 0x2: return  X - Y;
			case 0x3: return -X - Y;
			case 0x4: return  X + Z;
			case 0x5: return -X + Z;
			case 0x6: return  X - Z;
			case 0x7: return -X - Z;
			case 0x8: return  Y + Z;
			case 0x9: return -Y + Z;
			case 0xA: return  Y - Z;
			case 0xB: return -Y - Z;
			case 0xC: return  Y + X;
			case 0xD: return -Y + Z;
			case 0xE: return  Y - X;
			case 0xF: return -Y - Z;
		}
	}

};