#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/CatalystBaseMath.h>

class ValueNoise final
{

public:

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the given coordinates.
	*/
	static float Generate(const float X, const float Y, const float seed = 0.0f) NOEXCEPT
	{
		const float xSeeded{ CatalystBaseMath::Absolute(X + seed) };
		const float ySeeded{ CatalystBaseMath::Absolute(Y + seed) };

		const float XI{ CatalystBaseMath::Floor<float>(xSeeded) };
		const float YI{ CatalystBaseMath::Floor<float>(ySeeded) };
		const float XF{ CatalystBaseMath::Fractional(xSeeded) };
		const float YF{ CatalystBaseMath::Fractional(ySeeded) };

		const float A{ Random(XI, YI) };
		const float B{ Random(XI + 1.0f, YI) };
		const float C{ Random(XI, YI + 1.0f) };
		const float D{ Random(XI + 1.0f, YI + 1.0f) };

		const float xAlpha{ XF * XF * (3.0f - 2.0f * XF) };
		const float yAlpha{ YF * YF * (3.0f - 2.0f * YF) };

		return CatalystBaseMath::LinearlyInterpolate(A, B, xAlpha) + (C - A) * yAlpha * (1.0f - xAlpha) + (D - B) * xAlpha * yAlpha;
	}

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the given coordinates.
	*/
	static float GenerateNormalized(const float X, const float Y, const float seed = 0.0f) NOEXCEPT
	{
		return (Generate(X, Y, seed) + 1.0f) * 0.5f;
	}

private:

	/*
	*	Random function.
	*/
	static float Random(const float X, const float Y) NOEXCEPT
	{
		return CatalystBaseMath::Fractional(CatalystBaseMath::SineRadians(X * 12.9898f + Y * 78.233f) * 43'758.545312f);
	}
};