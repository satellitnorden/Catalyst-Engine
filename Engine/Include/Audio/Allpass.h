#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Allpass final
{

public:

	/*
	*	Initializes this allpass filter.
	*/
	FORCE_INLINE void Initialize(const float32 factor) NOEXCEPT
	{
		//Set the factor.
		_Factor = factor;

		//Set the previous sample.
		_PreviousSample = 0.0f;
	}

	/*
	*	Sets the factor.
	*/
	FORCE_INLINE void SetFactor(const float32 value) NOEXCEPT
	{
		_Factor = value;
	}

	/*
	*	Processes this allpass filter.
	*/
	FORCE_INLINE void Process(const float32 *const RESTRICT inputs, float32 *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			const float32 Y{ -_Factor * inputs[sample_index] + _PreviousSample };
			_PreviousSample = inputs[sample_index] + _Factor * Y;
			outputs[sample_index] = Y;
		}
	}

private:

	//The factor.
	float32 _Factor;

	//The previous sample.
	float32 _PreviousSample;

};