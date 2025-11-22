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
	float32 _Release{ 100.0f / 1'000.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE NoiseGate() NOEXCEPT
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
		//Iterate over the channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache the state.
			State &state{ _States[channel_index] };

			//Calculate properties that are note sample dependant.
			const uint32 attack_time_samples{ CalculateNumberOfSamples(_Attack, _SampleRate) };
			const float32 increase_per_sample{ 1.0f / static_cast<float32>(attack_time_samples) };

			const uint32 hold_time_samples{ CalculateNumberOfSamples(_Hold, _SampleRate) };
			const uint32 release_time_samples{ CalculateNumberOfSamples(_Release, _SampleRate) };
			const float32 decrease_per_sample{ 1.0f / static_cast<float32>(release_time_samples) };

			//Process all samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Calculate the absolute input sample.
				const float32 absolute_input_sample{ BaseMath::Absolute(inputs.At(channel_index).At(sample_index)) };

				//Calculate if the noise gate should be open.
				const bool should_be_open{ absolute_input_sample >= _Threshold };

				//If the noise gate should be open, increase the multiplier towards 1.0f over '_AttackTime'.
				{
					state._CurrentMultiplier = BaseMath::Minimum<float32>(state._CurrentMultiplier + (increase_per_sample * static_cast<float32>(should_be_open)), 1.0f);

					state._HoldSamples *= static_cast<uint32>(!should_be_open);
				}

				//If the noise gate should be closed, decrease the multiplier towards 0.0f over '_ReleaseTime' after '_HoldTime'.
				{
					const float32 hold_multiplier{ static_cast<float32>(state._HoldSamples >= hold_time_samples) };

					state._CurrentMultiplier = BaseMath::Maximum<float32>(state._CurrentMultiplier - (decrease_per_sample * static_cast<float32>(!should_be_open) * hold_multiplier), 0.0f);

					state._HoldSamples += 1 * static_cast<uint32>(!should_be_open);
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

		//The current multiplier.
		float32 _CurrentMultiplier{ 1.0f };

		//The hold samples.
		uint32 _HoldSamples{ 0 };

	};

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