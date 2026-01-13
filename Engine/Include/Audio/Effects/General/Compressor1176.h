#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Compressor that aims to replicate the sound of 1176 compressor units.
*/
class Compressor1176 final : public AudioEffect
{

public:

	//Enumeration covering all compressor ratios.
	enum class Ratio : uint32
	{
		OFF,
		FOUR_TO_ONE,
		EIGHT_TO_ONE,
		TWELVE_TO_ONE,
		TWENTY_TO_ONE,
		ALL_IN
	};

	//The input.
	float32 _Input{ 1.0f };

	//The output.
	float32 _Output{ 1.0f };

	//The compressor ratio.
	Ratio _Ratio{ Ratio::FOUR_TO_ONE };

	//The attack.
	float32 _Attack{ 3.5f };

	//The release.
	float32 _Release{ 3.5f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Compressor1176() NOEXCEPT
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
		//Define constants.
		constexpr float32 THRESHOLD{ -24.0f };
		constexpr float32 AVERAGE_GAIN_REDUCTION_TIME{ 10.0f / 1'000.0f };

		//Abort if compression is off.
		if (_Ratio == Ratio::OFF)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			}

			return;
		}

		//Copy inputs into outputs and apply the input gain.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, _Input);
		}

		//Calculate the average gain reduction coefficient.
		const float32 average_gain_reduction_coefficient{ std::exp(-1.0f / (AVERAGE_GAIN_REDUCTION_TIME * _SampleRate)) };

		//Calculate the ratio.
		float32 ratio{ 1.0f };

		switch (_Ratio)
		{
			case Ratio::FOUR_TO_ONE:
			{
				ratio = 4.0f;

				break;
			}

			case Ratio::EIGHT_TO_ONE:
			{
				ratio = 8.0f;

				break;
			}

			case Ratio::TWELVE_TO_ONE:
			{
				ratio = 12.0f;

				break;
			}

			case Ratio::TWENTY_TO_ONE:
			{
				ratio = 20.0f;

				break;
			}

			case Ratio::ALL_IN:
			{
				ratio = 1'000.0f;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Calculate the ratio reciprocal.
		const float32 ratio_reciprocal{ 1.0f / ratio };

		//Calculate the attack and release times.
		const float32 attack_time{ ExponentialInterpolation(20e-6f, 800e-6f, 1.0f - (_Attack / 7.0f)) };
		const float32 release_time{ ExponentialInterpolation(50e-3f, 1100e-3f, 1.0f - (_Release / 7.0f)) };

		//Calculate the attack and release coefficients.
		const float32 attack_coefficient{ std::exp(-std::log(9.0f) / (attack_time * _SampleRate)) };
		const float32 release_coefficient{ std::exp(-std::log(9.0f) / (release_time * _SampleRate)) };

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Retrieve the maximum input sample.
			float32 maximum_input_sample{ FLOAT32_EPSILON };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				maximum_input_sample = BaseMath::Maximum<float32>(maximum_input_sample, BaseMath::Absolute<float32>(outputs->At(channel_index).At(sample_index)) * Audio::DecibelsToGain(_PreviousGain));
			}

			//Calculate the input gain.
			const float32 input_gain{ Audio::GainToDecibels(maximum_input_sample) };

			//Calculate the desired gain.
			float32 desired_gain;

			if (input_gain > THRESHOLD)
			{
				desired_gain = THRESHOLD + ((input_gain - THRESHOLD) * ratio_reciprocal);
			}

			else
			{
				desired_gain = input_gain;
			}

			//Calculate the gain change.
			const float32 gain_change{ desired_gain - input_gain };

			//Calculate the final gain.
			float32 final_gain;

			if (gain_change < _PreviousGain)
			{
				final_gain = ((1.0f - attack_coefficient) * gain_change) + (attack_coefficient * _PreviousGain);
			}

			else
			{
				final_gain = ((1.0f - release_coefficient) * gain_change) + (release_coefficient * _PreviousGain);
			}

			//Write the outputs.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) = outputs->At(channel_index).At(sample_index) * Audio::DecibelsToGain(final_gain);
			}

			//Update the previous gain.
			_PreviousGain = final_gain;

			//Update the average gain reduction.
			_AverageGainReduction = ((1.0f - average_gain_reduction_coefficient) * final_gain) + (average_gain_reduction_coefficient * _AverageGainReduction);
		}

		//Apply the output.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, _Output);
		}
	}

	/*
	*	Returns the average gain reduction.
	*/
	FORCE_INLINE NO_DISCARD float32 GetAverageGainReduction() const NOEXCEPT
	{
		return _AverageGainReduction;
	}

private:

	//The previous gain.
	float32 _PreviousGain{ 0.0f };

	//The average gain reduction.
	float32 _AverageGainReduction{ 0.0f };

	/*
	*	Performs an exponential interpolation.
	*/
	FORCE_INLINE NO_DISCARD float32 ExponentialInterpolation(const float32 A, const float32 B, const float32 power) NOEXCEPT
	{
		return A * std::pow(B / A, power);
	}

};