#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Simple compressor audio effect.
*/
class Compressor final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_THRESHOLD{ 1.0f };
	constexpr static float32 DEFAULT_RATIO{ 2.0f };
	constexpr static float32 DEFAULT_ATTACK{ 20.0f / 1'000.0f };
	constexpr static float32 DEFAULT_RELEASE{ 100.0f / 1'000.0f };
	constexpr static float32 DEFAULT_MAKEUP{ 1.0f };

	//The threshold.
	float32 _Threshold{ DEFAULT_THRESHOLD };

	//The ratio.
	float32 _Ratio{ DEFAULT_RATIO };

	//The attack.
	float32 _Attack{ DEFAULT_ATTACK };

	//The release.
	float32 _Release{ DEFAULT_RELEASE };

	//The makeup.
	float32 _Makeup{ DEFAULT_MAKEUP };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Compressor(const bool instantaneous = false) NOEXCEPT
		:
		_Instantaneous(instantaneous)
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
		//Reset if parameters changed.
		if (_PreviousThreshold != _Threshold
			|| _PreviousRatio != _Ratio
			|| _PreviousAttack != _Attack
			|| _PreviousRelease != _Release)
		{
			Reset();
		}

		//Process all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Reset the gain reduction.
			_GainReductions[channel_index] = 0.0f;

			//If the ratio is one, just copy the inputs and move on.
			if (_Ratio <= 1.0f)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);

				continue;
			}

			//Calculate the threshold in dB.
			const float32 threshold{ Audio::GainToDecibels(_Threshold) };

			//Process all samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Cache the input sample.
				const float32 input_sample{ inputs.At(channel_index).At(sample_index) };

				//Calculate gain.
				float32 gain{ 0.0f };

				if (_Instantaneous)
				{
					gain = Audio::GainToDecibels(input_sample);
				}

				else
				{
					_RMSStates[channel_index] = _RMSCoefficient * _RMSStates[channel_index] + (1.0f - _RMSCoefficient) * (input_sample * input_sample);
					gain = Audio::GainToDecibels(std::sqrt(_RMSStates[channel_index]));
				}

				//Calculate the desired gain.
				float32 desired_gain{ 0.0f };

				if (gain > threshold)
				{
					const float32 over{ gain - threshold };
					const float32 compressed{ over / _Ratio };
					const float32 gain_change{ compressed - over };

					desired_gain = gain_change;
				}

				//Update the envelope state.
				if (desired_gain < _EnvelopeStates[channel_index])
				{
					_EnvelopeStates[channel_index] = _AttackCoefficient * _EnvelopeStates[channel_index] + (1.0f - _AttackCoefficient) * desired_gain;
				}

				else
				{
					_EnvelopeStates[channel_index] = _ReleaseCoefficient * _EnvelopeStates[channel_index] + (1.0f - _ReleaseCoefficient) * desired_gain;
				}

				//Calculate the multiplier.
				const float32 multiplier{ Audio::DecibelsToGain(_EnvelopeStates[channel_index]) };

				//Write the output.
				outputs->At(channel_index).At(sample_index) = input_sample * multiplier * _Makeup;

				//Add to the gain reduction.
				_GainReductions[channel_index] += multiplier;
			}

			//Normalize the gain reduction.
			_GainReductions[channel_index] /= static_cast<float32>(number_of_samples);
		}
	}

	/*
	*	Returns the gain reduction for the given channel.
	*/
	FORCE_INLINE NO_DISCARD float32 GetGainReduction(const uint8 channel_index) NOEXCEPT
	{
		return _GainReductions[channel_index];
	}

private:

	//Denotes whether or not this compressor is instantaneous.
	bool _Instantaneous{ false };

	//The previous threshold.
	float32 _PreviousThreshold{ DEFAULT_THRESHOLD };

	//The previous ratio.
	float32 _PreviousRatio{ DEFAULT_RATIO };

	//The aprevious ttack.
	float32 _PreviousAttack{ DEFAULT_ATTACK };

	//The previous release.
	float32 _PreviousRelease{ DEFAULT_RELEASE };

	//The envelope states (in dB).
	StaticArray<float32, 2> _EnvelopeStates{ 0.0f };

	//The attack coefficient.
	float32 _AttackCoefficient{ 0.0f };

	//The release coefficient.
	float32 _ReleaseCoefficient{ 0.0f };

	//The RMS states.
	StaticArray<float32, 2> _RMSStates{ 0.0f };

	//The RMS coefficient.
	float32 _RMSCoefficient{ 0.0f };

	//The gain reductions.
	StaticArray<float32, 2> _GainReductions;

	/*
	*	Calculates a coefficient with the given value.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateCoefficient(const float32 value) NOEXCEPT
	{
		return std::exp(-1.0f / (value * _SampleRate));
	}

	/*
	*	Resets this effect.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		//Calculate the attack coefficient.
		_AttackCoefficient = CalculateCoefficient(_Attack);

		//Calculate the attreleaseack coefficient.
		_ReleaseCoefficient = CalculateCoefficient(_Release);

		//Calculate the RMS coefficient.
		_RMSCoefficient = CalculateCoefficient(20.0f / 1'000.0f);

		//Reset the previous values.
		_PreviousThreshold = _Threshold;
		_PreviousRatio = _Ratio;
		_PreviousAttack = _Attack;
		_PreviousRelease = _Release;
	}

};