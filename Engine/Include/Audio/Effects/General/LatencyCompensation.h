#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Simple class that delays the signal a set amount of time, to align with other signals.
*/
class LatencyCompensation final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LatencyCompensation() NOEXCEPT
	{
		SetCompensation(_CompensationTime);
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		SetCompensation(_CompensationTime);
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
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Don't apply any compensation if there's none to be applied.
			if (_NumberOfCompensationSamples == 0)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			}

			else
			{
				for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
				{
					const float32 input_sample{ inputs.At(channel_index).At(sample_index) };
					outputs->At(channel_index).At(sample_index) = _Buffers.At(channel_index).At(_Counters[channel_index]);
					_Buffers.At(channel_index).At(_Counters[channel_index]) = input_sample;

					++_Counters[channel_index];
					_Counters[channel_index] *= static_cast<uint32>(_Counters[channel_index] < _NumberOfCompensationSamples);
				}
			}
		}
	}

	/*
	*	Sets the compensation.
	*/
	FORCE_INLINE void SetCompensation(const float32 compensation) NOEXCEPT
	{
		Memory::Set(_Counters.Data(), 0, 2 * sizeof(uint32));

		_CompensationTime = compensation;
		_NumberOfCompensationSamples = Audio::TimeToSamples(_CompensationTime, _SampleRate);

		if (_NumberOfCompensationSamples == 0)
		{
			return;
		}

		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			_Buffers[channel_index].Resize<false>(_NumberOfCompensationSamples);
			Memory::Set(_Buffers[channel_index].Data(), 0, _NumberOfCompensationSamples * sizeof(float32));
		}
	}

private:

	//The compensation time.
	float32 _CompensationTime{ 0.0f };

	//The number of compensation samples.
	uint32 _NumberOfCompensationSamples{ 0 };

	//The buffers.
	StaticArray<DynamicArray<float32>, 2> _Buffers;

	//The counters.
	StaticArray<uint32, 2> _Counters;

};