#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Core/BaseMath.h>

//STD.
#include <cmath>

/*
*	Class encapsulating a dual delay line.
*	This samples the signal at the given playback rate with two overlapping voices, so it can be used to implement pitch shifting.
*/
class DualDelayLine final
{

public:

	/*
	*	Initializes this dual delay line.
	*/
	FORCE_INLINE void Initialize(const float32 delay_time, const float32 sample_rate, const float32 playback_rate) NOEXCEPT
	{
		//Calculate the voice samples.
		_VoiceSamples = static_cast<uint32>(delay_time * sample_rate);
		_VoiceSamplesReciprocal = 1.0f / static_cast<float32>(_VoiceSamples);

		//Calculate the buffer size.
		const uint64 buffer_size{ static_cast<uint64>(static_cast<float32>(_VoiceSamples) * playback_rate)  };

		//Initialize the buffer.
		_Buffer.Upsize<false>(buffer_size);
		Memory::Set(_Buffer.Data(), 0, sizeof(float32) * buffer_size);

		//Initialize the current index.
		_CurrentIndex = 0;

		//Initialize the voice positions.
		_Voice1Position = 0.0f;
		_Voice2Position = 0.0f;

		//Initialize the voice phases.
		_Voice1Phase = 0;
		_Voice2Phase = _VoiceSamples / 2;

		//Set the playback rate.
		_PlaybackRate = playback_rate;
	}

	/*
	*	Processes this delay line.
	*/
	FORCE_INLINE void Process(const float32 *const RESTRICT inputs, float32 *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			outputs[sample_index] = ProcessSingle(inputs[sample_index]);
		}
	}

	/*
	*	Processes this delay line with a single sample.
	*/
	FORCE_INLINE NO_DISCARD float32 ProcessSingle(const float32 input_sample) NOEXCEPT
	{
		//Write the input.
		_Buffer[_CurrentIndex] = input_sample;

		//Sample the voices.
		const float32 voice_1{ SampleVoice(_Voice1Position, _Voice1Phase) };
		const float32 voice_2{ SampleVoice(_Voice2Position, _Voice2Phase) };

		//Update the current index.
		++_CurrentIndex;
		_CurrentIndex -= _Buffer.Size() * static_cast<uint64>(_CurrentIndex >= _Buffer.Size());

		//Return the output.
		return voice_1 + voice_2;
	}

private:

	//The voice samples.
	uint32 _VoiceSamples;

	//The voice samples reciprocal.
	float32 _VoiceSamplesReciprocal;

	//The buffer.
	DynamicArray<float32> _Buffer;

	//The current index.
	uint64 _CurrentIndex;

	//The voice positions.
	float32 _Voice1Position;
	float32 _Voice2Position;

	//The voice phases.
	uint32 _Voice1Phase;
	uint32 _Voice2Phase;

	//The playback rate.
	float32 _PlaybackRate;

	/*
	*	Samples a voice.
	*/
	FORCE_INLINE NO_DISCARD float32 SampleVoice(float32 &voice_position, uint32 &voice_phase) NOEXCEPT
	{
		//Calculate the phase scalar.
		const float32 phase_scalar{ static_cast<float32>(voice_phase) * _VoiceSamplesReciprocal };

		//Calculate the window.
		const float32 window{ 0.5f * (1.0f - std::cos(2.0f * BaseMathConstants::PI * phase_scalar)) };

		//Calculate the sample.
		uint32 index_0{ static_cast<uint32>(voice_position) };
		index_0 -= static_cast<uint32>(_Buffer.Size()) * static_cast<uint32>(index_0 >= _Buffer.Size());
		uint32 index_1{ index_0 + 1 };
		index_1 -= static_cast<uint32>(_Buffer.Size()) * static_cast<uint32>(index_1 >= _Buffer.Size());
		const float32 alpha{ BaseMath::Fractional(voice_position) };

		const float32 sample{ BaseMath::LinearlyInterpolate(_Buffer[index_0], _Buffer[index_1], alpha) };

		//Update the position/phase.
		voice_position += _PlaybackRate;
		++voice_phase;

		if (voice_phase >= _VoiceSamples)
		{
			voice_position = static_cast<float32>(_CurrentIndex + 2);
			voice_phase = 0;
		}

		//Return the sample!
		return sample * window;
	}

};