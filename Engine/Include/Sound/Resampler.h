#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

class Resampler final
{

public:

	//Type aliases.
	using OutputFunction = void(*)(const float32 sample, void *const RESTRICT user_data);

	/*
	*	Initializes this resampler.
	*/
	FORCE_INLINE void Initialize
	(
		const float32 original_sample_rate,
		const float32 target_sample_rate,
		const OutputFunction output_function,
		void *const RESTRICT user_data
	) NOEXCEPT
	{
		//Calculate the ratio.
		_Ratio = target_sample_rate / original_sample_rate;

		//Set the phase.
		_Phase = 0.0f;

		//Set the previous sample.
		_PreviousSample = 0.0f;

		//Set the output function.
		_OutputFunction = output_function;

		//Set the user data.
		_UserData = user_data;
	}

	/*
	*	Processes this resampler.
	*/
	FORCE_INLINE void Process(const float32 input_sample) NOEXCEPT
	{
		if (_Phase >= 0.0f)
		{
			const float32 interpolated_sample{ BaseMath::LinearlyInterpolate(_PreviousSample, input_sample, _Phase) };

			_OutputFunction(interpolated_sample, _UserData);

			_Phase -= 1.0f;
		}

		_Phase += _Ratio;
		_PreviousSample = input_sample;
	}

private:

	//The ratio.
	float32 _Ratio;

	//The phase.
	float32 _Phase;

	//The previous sample.
	float32 _PreviousSample;

	//The output function.
	OutputFunction _OutputFunction{ nullptr };

	//The user data.
	void *RESTRICT _UserData;

};