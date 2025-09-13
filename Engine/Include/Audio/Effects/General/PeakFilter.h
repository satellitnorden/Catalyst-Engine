#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Biquad.h>

class PeakFilter final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_FREQUENCY{ 1'000.0f };
	constexpr static float32 DEFAULT_GAIN{ 1.0f };
	constexpr static float32 DEFAULT_QUALITY{ 1.0f };

	//The frequency.
	float32 _Frequency{ DEFAULT_FREQUENCY };

	//The gain.
	float32 _Gain{ DEFAULT_GAIN };

	//The quality.
	float32 _Quality{ DEFAULT_QUALITY };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PeakFilter(const float32 frequency = DEFAULT_FREQUENCY, const float32 gain = DEFAULT_GAIN, const float32 quality = DEFAULT_QUALITY) NOEXCEPT
		:
		_Frequency(frequency),
		_Gain(gain),
		_Quality(quality)
	{
		//Reset.
		Reset();
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Reset.
		Reset();
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
		//Reset if the frequency/gain/quality has changed
		if (_PreviousFrequency != _Frequency
			|| _PreviousGain != _Gain
			|| _PreviousQuality != _Quality)
		{
			//Reset.
			Reset();

			//Update the previous values.
			_PreviousFrequency = _Frequency;
			_PreviousGain = _Gain;
			_PreviousQuality = _Quality;
		}

		//Process the samples.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_Biquads[channel_index].Process(inputs.At(channel_index).Data(), outputs->At(channel_index).Data(), number_of_samples);
		}
	}

private:

	//The previous frequency.
	float32 _PreviousFrequency{ DEFAULT_FREQUENCY };

	//The previous gain.
	float32 _PreviousGain{ DEFAULT_GAIN };

	//The previous quality.
	float32 _PreviousQuality{ DEFAULT_QUALITY };

	//The biquads.
	StaticArray<Biquad, 2> _Biquads;

	/*
	*	Resets this peak filter.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		for (Biquad &biquad : _Biquads)
		{
			biquad.InitializePeak(_Frequency, _Gain, _Quality, _SampleRate);
		}
	}

};