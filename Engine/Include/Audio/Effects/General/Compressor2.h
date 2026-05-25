#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Simple compressor audio effect.
*/
class Compressor2 final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_THRESHOLD{ 1.0f };
	constexpr static float32 DEFAULT_KNEE{ 1.0f };
	constexpr static float32 DEFAULT_RATIO{ 2.0f };
	constexpr static float32 DEFAULT_ATTACK{ 20.0f / 1'000.0f };
	constexpr static float32 DEFAULT_RELEASE{ 100.0f / 1'000.0f };
	constexpr static float32 DEFAULT_MAKEUP{ 1.0f };

	//The threshold.
	float32 _Threshold{ DEFAULT_THRESHOLD };

	//The knee.
	float32 _Knee{ DEFAULT_KNEE };

	//The ratio.
	float32 _Ratio{ DEFAULT_RATIO };

	//The attack.
	float32 _Attack{ DEFAULT_ATTACK };

	//The release.
	float32 _Release{ DEFAULT_RELEASE };

	//The makeup.
	float32 _Makeup{ DEFAULT_MAKEUP };

	//The power.
	bool _Power{ true };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Compressor2() NOEXCEPT
	{
		//Define constants.
		constexpr float32 GAIN_REDUCTION_TIME{ 10.0f / 1'000.0f };

		//Calculate the gain reduction coefficient.
		_GainReductionCoefficient = std::exp(-1.0f / (GAIN_REDUCTION_TIME * _SampleRate));
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

		//If the power isn't on, copy the inputs into the outputs and move in.
		if (!_Power)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			}

			_GainReduction = 0.0f;

			return;
		}

		//Calculate the detector envelope coefficient.
		const float32 detector_envelope_coefficient{ std::exp(-1.0f / (DETECTOR_ENVELOPE_TIME * _SampleRate)) };

		//Calculate the attack coefficient.
		const float32 attack_coefficient{ std::exp(-1.0f / (_Attack * _SampleRate)) };

		//Calculate the release coefficient.
		const float32 release_coefficient{ std::exp(-1.0f / (_Release * _SampleRate)) };

		//Calculate the threshold.
		const float32 threshold{ Audio::GainToDecibels(_Threshold) };

		//Calculate the knee.
		const float32 knee{ Audio::GainToDecibels(_Knee) };

		//Calculate the half knee.
		const float32 knee_half{ knee * 0.5f };

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Calculate the detector input.
			float32 detector_input{ FLOAT32_EPSILON };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				detector_input = BaseMath::Maximum<float32>(detector_input, BaseMath::Absolute(inputs.At(channel_index).At(sample_index)));
			}

			//Update the detector envelope.
			_DetectorEnvelope = BaseMath::Maximum<float32>
			(
				detector_input,
				(_DetectorEnvelope * detector_envelope_coefficient) + (detector_input * (1.0f - detector_envelope_coefficient))
			);

			//Calculate gain.
			const float32 gain{ Audio::GainToDecibels(_DetectorEnvelope) };

			//Calculate the desired gain.
			const float32 desired_gain{ CalculateDesiredGain(gain, threshold, knee, knee_half) };

			//Update the envelope state.
			if (desired_gain < _EnvelopeState)
			{
				_EnvelopeState = attack_coefficient * _EnvelopeState + (1.0f - attack_coefficient) * desired_gain;
			}

			else
			{
				_EnvelopeState = release_coefficient * _EnvelopeState + (1.0f - release_coefficient) * desired_gain;
			}

			//Calculate the multiplier.
			const float32 multiplier{ Audio::DecibelsToGain(_EnvelopeState) };

			//Write the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index) * multiplier * _Makeup;
			}

			//Update the gain reduction.
			_GainReduction = _GainReduction * _GainReductionCoefficient + -_EnvelopeState * (1.0f - _GainReductionCoefficient);
		}
	}

	/*
	*	Returns the gain reduction for the given channel.
	*/
	FORCE_INLINE NO_DISCARD float32 GetGainReduction() NOEXCEPT
	{
		return _GainReduction;
	}

private:

	//The detector envelope.
	float32 _DetectorEnvelope{ FLOAT32_EPSILON };

	//The envelope state.
	float32 _EnvelopeState{ FLOAT32_EPSILON };

	//The gain reduction coefficient.
	float32 _GainReductionCoefficient{ 0.0f };

	//The gain reduction.
	float32 _GainReduction{ 0.0f };

	/*
	*	Calculates the desired gain.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateDesiredGain
	(
		const float32 input_gain,
		const float32 threshold,
		const float32 knee,
		const float32 knee_half
	) NOEXCEPT
	{
		if (input_gain < threshold - knee_half)
		{
			return 0.0f;
		}
		else if (input_gain > threshold + knee_half)
		{
			return (threshold + (input_gain - threshold) / _Ratio) - input_gain;
		}
		else
		{
			const float32 D{ input_gain - threshold + knee_half };
			return (1.0f / _Ratio - 1.0f) * D * D / (2.0f * knee);
		}
	}

};