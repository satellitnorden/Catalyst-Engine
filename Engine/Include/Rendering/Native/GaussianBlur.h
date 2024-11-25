#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/Core/BaseMath.h>

class GaussianBlur final
{

public:

	/*
	*	Calculates weights for a (separable) gaussian blur filter with the given sigma.
	*/
	FORCE_INLINE static void CalculateWeights(ArrayProxy<float32> *const RESTRICT weights, const float32 sigma = 1.0f) NOEXCEPT
	{
		float32 R{ 2.0f * sigma * sigma };
		const float32 S{ 2.0f * sigma * sigma };

		float32 sum{ 0.0f };

		int32 weights_offset{ (static_cast<int32>(weights->Size()) - 1) / 2 };

		for (int32 X{ -weights_offset }; X <= weights_offset; ++X)
		{
			R = BaseMath::SquareRoot(static_cast<float32>(X * X));
			(*weights)[X + weights_offset] = (BaseMath::Exponential(-(R * R) / S)) / (BaseMathConstants::PI * S);
			sum += (*weights)[X + weights_offset];
		}

		for (float32 &value : *weights)
		{
			value /= sum;
		}
	}
};