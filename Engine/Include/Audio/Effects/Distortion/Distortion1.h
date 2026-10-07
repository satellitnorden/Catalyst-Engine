#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/OverSampler.h>

/*
*	A distortion that generates an asymmetric soft-ish clipping curve.
*/
class Distortion1 final : public AudioEffect
{

public:

	//The gain.
	float32 _Gain;

	//The minimum gain.
	float32 _MinimumGain;

	//The maximum gain.
	float32 _MaximumGain;

	//The feedback.
	float32 _Feedback;

	//The post gain.
	float32 _PostGain;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Distortion1(const float32 gain = 0.5f, const float32 minimum_gain = 40.0f, const float32 maximum_gain = 80.0f, const float32 feedback = 0.0f, const float32 post_gain = Audio::DecibelsToGain(-24.0f)) NOEXCEPT
		:
		_Gain(gain),
		_MinimumGain(minimum_gain),
		_MaximumGain(maximum_gain),
		_Feedback(feedback),
		_PostGain(post_gain)
	{

	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//Calculate the gain.
		const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(_MinimumGain, _MaximumGain, BaseMath::InverseSquare(_Gain)))};

		//Iterate over all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Iterate over all samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Feed the oversampler.
				_OverSamplers[channel_index].Feed(inputs.At(channel_index).At(sample_index));

				//Process each sample.
				for (float32 &sample : _OverSamplers[channel_index].GetSamples())
				{
					//Cache the input sample.
					const float32 input_sample{ (sample * gain) + (_PreviousSamples[channel_index] * _Feedback) };

					//Calculate the interpolator.
					const float32 interpolator{ std::tanh(input_sample) };

					//Calculate the normalized interpolator.
					const float32 normalized_interpolator{ interpolator * 0.5f + 0.5f };

					//Calculate the upper distortion.
					const float32 upper_distortion{ BaseMath::LinearlyInterpolate(std::atan(input_sample), interpolator, std::pow(normalized_interpolator, 64.0f)) };

					//Calculate the lower distortion.
					const float32 lower_distortion{ interpolator };

					//Blend between the distortions.
					const float32 blend{ BaseMath::LinearlyInterpolate(lower_distortion, upper_distortion, normalized_interpolator) };

					//Write the sample.
					sample = blend * _PostGain;

					//Write the previous sample.
					_PreviousSamples[channel_index] = blend;
				}

				//Write to the buffer!
				outputs->At(channel_index).At(sample_index) = _OverSamplers[channel_index].DownSample();
			}
		}
	}

private:

	//The oversamplers.
	StaticArray<OverSampler<2, 32>, 2> _OverSamplers;

	//The previous samples.
	StaticArray<float32, 2> _PreviousSamples{ 0.0f, 0.0f };

};