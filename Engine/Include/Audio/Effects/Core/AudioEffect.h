#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/AudioProcessContext.h>
#include <Audio/AudioCore.h>

/*
*	An audio effects encapsulates one piece of audio processing, and can be used as modular building blocks to create more complex plugins.
*	Shares some interfaces with 'AudioPlugin' to be able to be easily used.
*/
class AudioEffect
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioEffect() NOEXCEPT
	{

	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 sample_rate) NOEXCEPT
	{
		if (_SampleRate != sample_rate)
		{
			_SampleRate = sample_rate;
			OnSampleRateChanged();
		}
	}

	/*
	*	Sets the beats per minute.
	*/
	FORCE_INLINE void SetBeatsPerMinute(const float32 beats_per_minute) NOEXCEPT
	{
		if (_BeatsPerMinute != beats_per_minute)
		{
			_BeatsPerMinute = beats_per_minute;
			OnBeatsPerMinuteChanged();
		}
	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE virtual void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index);
			}
		}
	}

protected:

	//The sample rate.
	float32 _SampleRate{ Audio::DEFAULT_SAMPLE_RATE };

	//The beats per minute.
	float32 _BeatsPerMinute{ Audio::DEFAULT_BEATS_PER_MINUTE };

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE virtual void OnSampleRateChanged() NOEXCEPT
	{

	}

	/*
	*	Callback for when the beats per minute changed.
	*/
	FORCE_INLINE virtual void OnBeatsPerMinuteChanged() NOEXCEPT
	{

	}

};