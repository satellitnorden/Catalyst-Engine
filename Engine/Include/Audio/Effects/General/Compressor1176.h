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
		constexpr float32 DETECTOR_ENVELOPE_TIME{ 1.0f / 1'000.0f };
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

		//Calculate the detector envelope coefficient.
		const float32 detector_envelope_coefficient{ std::exp(-1.0f / (DETECTOR_ENVELOPE_TIME * _SampleRate)) };

		//Calculate the average gain reduction coefficient.
		const float32 average_gain_reduction_coefficient{ std::exp(-1.0f / (AVERAGE_GAIN_REDUCTION_TIME * _SampleRate)) };

		//Calculate the attack and release times.
		const float32 attack_time{ ExponentialInterpolation(0.02f / 1'000.0f, 0.8f / 1'000.0f, 1.0f - (_Attack / 7.0f)) };
		const float32 release_time{ ExponentialInterpolation(50.0f / 1'000.0f, 1'100.0f / 1'000.0f, 1.0f - (_Release / 7.0f)) };

		//Calculate the attack and release coefficients.
		const float32 attack_coefficient{ std::exp(-std::log(9.0f) / (attack_time * _SampleRate)) };
		const float32 release_coefficient{ std::exp(-std::log(9.0f) / (release_time * _SampleRate)) };

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Write the outputs using the previous gain, and update the detector input.
			_DetectorInput = FLOAT32_EPSILON;

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) = outputs->At(channel_index).At(sample_index) * _PreviousGain;
				_DetectorInput = BaseMath::Maximum<float32>(_DetectorInput, BaseMath::Absolute<float32>(outputs->At(channel_index).At(sample_index)));
			}

			//Update the detector envelope.
			_DetectorEnvelope = BaseMath::LinearlyInterpolate
			(
				((1.0f - detector_envelope_coefficient) * _DetectorInput) + (detector_envelope_coefficient * _DetectorEnvelope),
				_DetectorInput,
				static_cast<float32>(_DetectorInput > _DetectorEnvelope)
			);

			//Calculate the desired gain.
			const float32 desired_gain{ DesiredGain(_DetectorEnvelope, _Ratio) };

			//Calculate the final gain.
			float32 final_gain;

			if (desired_gain < _PreviousGain)
			{
				final_gain = ((1.0f - attack_coefficient) * desired_gain) + (attack_coefficient * _PreviousGain);
			}

			else
			{
				final_gain = ((1.0f - release_coefficient) * desired_gain) + (release_coefficient * _PreviousGain);
			}

			//Clamp for safety. (:
			final_gain = BaseMath::Clamp<float32>(final_gain, FLOAT32_EPSILON, 1.0f);

			//Update the previous gain.
			_PreviousGain = final_gain;

			//Update the average gain reduction.
			_AverageGainReduction = ((1.0f - average_gain_reduction_coefficient) * Audio::GainToDecibels(final_gain)) + (average_gain_reduction_coefficient * _AverageGainReduction);
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

	//The detector input.
	float32 _DetectorInput{ FLOAT32_EPSILON };

	//The detector envelope.
	float32 _DetectorEnvelope{ FLOAT32_EPSILON };

	//The previous gain.
	float32 _PreviousGain{ 1.0f };

	//The average gain reduction.
	float32 _AverageGainReduction{ 0.0f };

	/*
	*	Performs an exponential interpolation.
	*/
	FORCE_INLINE NO_DISCARD float32 ExponentialInterpolation(const float32 A, const float32 B, const float32 power) NOEXCEPT
	{
		return A * std::pow(B / A, power);
	}

	/*
	*	Calculates the desired gain.
	*/
	FORCE_INLINE NO_DISCARD float32 DesiredGain(const float32 detector_envelope, const Ratio ratio) NOEXCEPT
	{
		//Calculate the alpha/beta.
		float32 alpha;
		float32 beta;

		switch (ratio)
		{
			case Ratio::FOUR_TO_ONE:
			{
				alpha = 4.0f;
				beta = 1.0f;

				break;
			}

			case Ratio::EIGHT_TO_ONE:
			{
				alpha = 8.0f;
				beta = 1.2f;

				break;
			}

			case Ratio::TWELVE_TO_ONE:
			{
				alpha = 12.0f;
				beta = 1.4f;

				break;
			}

			case Ratio::TWENTY_TO_ONE:
			{
				alpha = 20.0f;
				beta = 1.6f;

				break;
			}

			case Ratio::ALL_IN:
			{
				alpha = 40.0f;
				beta = 2.0f;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Calculate the all in "chaos".
		const float32 chaos{ detector_envelope * 0.02f * static_cast<float32>(ratio == Ratio::ALL_IN) };

		//Apply the curve!
		return 1.0f / (1.0f + alpha * std::pow(detector_envelope + chaos, beta));
	}

};