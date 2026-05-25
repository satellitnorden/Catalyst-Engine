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

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Distortion1(const float32 gain = 0.5f) NOEXCEPT
		:
		_Gain(gain)
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
		const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(40.0f, 80.0f, BaseMath::InverseSquare(_Gain)))};

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
					//Calculate the interpolator.
					const float32 interpolator{ std::tanh(sample * gain) };

					//Calculate the normalized interpolator.
					const float32 normalized_interpolator{ interpolator * 0.5f + 0.5f };

					//Calculate the upper distortion.
					const float32 upper_distortion{ BaseMath::LinearlyInterpolate(std::atan(sample * gain), interpolator, std::pow(normalized_interpolator, 64.0f)) };

					//Calculate the lower distortion.
					const float32 lower_distortion{ interpolator };

					//Blend between the distortions.
					const float32 blend{ BaseMath::LinearlyInterpolate(lower_distortion, upper_distortion, normalized_interpolator) };

					//Write the sample.
					sample = blend * Audio::DecibelsToGain(-24.0f);
				}

				//Write to the buffer!
				outputs->At(channel_index).At(sample_index) = _OverSamplers[channel_index].DownSample();
			}
		}
	}

private:

	//The oversamplers.
	StaticArray<OverSampler<2, 32>, 2> _OverSamplers;

};