#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//Third party.
#include <ThirdParty/SignalSmithStretch/signalsmith-stretch.h>

/*
*	This effect implements a pitch shift effect, and can be controlled by 'coarse' steps (integer semitones) and 'fine' steps (decimal semitones).
*/
class PitchShift final : public AudioEffect
{

public:

	//The coarse.
	int32 _Coarse{ 0 };

	//The fine.
	float32 _Fine{ 0.0f };

	//The mix.
	float32 _Mix{ 1.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PitchShift(const bool low_latency = false) NOEXCEPT
	{
		//Configure the stretch.
		if (low_latency)
		{
			_Stretch.configure(2, _SampleRate * 0.1f, _SampleRate * 0.02f, false);
		}
		
		else
		{
			_Stretch.presetDefault(2, _SampleRate);
		}
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
		//Calculate the total pitch shift.
		const float32 total_pitch_shift{ (static_cast<float32>(_Coarse) + _Fine) };

		//If there's no pitch shift, just copy the input into the output and return.
		if (total_pitch_shift == 0.0f)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				//Process!
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			}

			return;
		}

		//Set the pitch shift.
		_Stretch.setTransposeSemitones(total_pitch_shift);

		//Copy the input into the input buffers.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			DynamicArray<float32>& input_buffer{ _InputBuffers[channel_index] };
			input_buffer.Resize<false>(number_of_samples);
			Memory::Copy(input_buffer.Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Process!
		_Stretch.process(_InputBuffers, number_of_samples, (*outputs), number_of_samples);

		//Apply the mix.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				outputs->At(channel_index).At(sample_index) = Audio::Crossfade(_InputBuffers[channel_index][sample_index], outputs->At(channel_index).At(sample_index), _Mix);
			}
		}
	}

private:

	//The input buffers.
	StaticArray<DynamicArray<float32>, 2> _InputBuffers;

	//The stretch.
	signalsmith::stretch::SignalsmithStretch<float32> _Stretch;

};