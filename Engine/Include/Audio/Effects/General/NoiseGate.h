#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/BaseMath.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	A simple noise gate. Has generally fast release and super fast attack. Goes down all the way to zero.
*/
class NoiseGate final : public AudioEffect
{

public:

	//The threshold.
	float32 _Threshold{ Audio::DecibelsToGain(-80.0f) };

	//The attack.
	float32 _Attack{ 1.0f / 1'000.0f };

	//The hold.
	float32 _Hold{ 10.0f / 1'000.0f };

	//The release.
	float32 _Release{ 40.0f / 1'000.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE NoiseGate() NOEXCEPT
	{
		//Define constants.
		constexpr float32 ENVELOPE_TIME{ 5.0f / 1'000.0f };

		//Calculate the envelope coefficient.
		_EnvelopeCoefficient = std::exp(-1.0f / (ENVELOPE_TIME * _SampleRate));
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
		constexpr float32 HYSTERESIS_MULTIPLIER{ 0.5f };

		//Define the gate floor.
		const float32 gate_floor{ Audio::DecibelsToGain(-80.0f) };

		//Calculate the hold samples.
		const uint32 hold_samples{ Audio::TimeToSamples(_Hold, _SampleRate) };

		//Calculate the attack and release coefficients.
		const float32 attack_coefficient{ std::exp(-1.0f / (_Attack * _SampleRate)) };
		const float32 release_coefficient{ std::exp(-1.0f / (_Release * _SampleRate)) };

		//Iterate over the channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache the state.
			State &state{ _States[channel_index] };

			//Process all samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Calculate the absolute input sample.
				const float32 absolute_input_sample{ BaseMath::Absolute(inputs.At(channel_index).At(sample_index)) };

				//Update the envelope.
				state._Envelope = state._Envelope * _EnvelopeCoefficient + absolute_input_sample * (1.0f - _EnvelopeCoefficient);

				//Update whether or not the gate is open or not.
				state._IsOpen = state._IsOpen ? state._Envelope >= (_Threshold * HYSTERESIS_MULTIPLIER) : state._Envelope >= _Threshold;

				//Update the number of hold samples.
				if (state._IsOpen)
				{
					state._NumberOfHoldSamples = 0;
				}

				else
				{
					++state._NumberOfHoldSamples;
				}

				//Update the current multiplier.
				if (state._IsOpen)
				{
					state._CurrentMultiplier = state._CurrentMultiplier * attack_coefficient + 1.0f * (1.0f - attack_coefficient);
				}

				else if (state._NumberOfHoldSamples >= hold_samples)
				{
					state._CurrentMultiplier = state._CurrentMultiplier * release_coefficient + gate_floor * (1.0f - release_coefficient);
				}

				//Apply the multiplier!
				outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index) * state._CurrentMultiplier;
			}
		}
	}

private:

	/*
	*	Noise gate state class definition.
	*/
	class State final
	{

	public:

		//Denotes whether the gate is open or not.
		bool _IsOpen{ false };

		//The envelope.
		float32 _Envelope{ 0.0f };

		//The number of hold samples.
		uint32 _NumberOfHoldSamples{ 0 };

		//The current multiplier.
		float32 _CurrentMultiplier{ 1.0f };

	};

	//The envelope coefficient.
	float32 _EnvelopeCoefficient{ 0.0f };

	//The states
	StaticArray<State, 2> _States;

	/*
	*	Calculates number of samples.
	*/
	FORCE_INLINE NO_DISCARD uint32 CalculateNumberOfSamples(const float32 seconds, const float32 sample_rate) const NOEXCEPT
	{
		return BaseMath::Round<uint32>(seconds * sample_rate);
	}

};