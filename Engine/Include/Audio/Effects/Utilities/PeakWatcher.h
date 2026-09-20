#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//Systems.
#include <Systems/LogSystem.h>

/*
*	Peak watcher audio effect.
*	Utility effect that keeps track of the highest peak encounted, and logs every time a higher peak is encountered.
*/
class PeakWatcher final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PeakWatcher(const char *const RESTRICT name) NOEXCEPT
		:
		_Name(name)
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
		//Update the highest peak.
		float32 highest_peak{ 0.0f };

		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				highest_peak = BaseMath::Maximum(highest_peak, BaseMath::Absolute(inputs.At(channel_index).At(sample_index)));
			}
		}

		if (_HighestPeak < highest_peak)
		{
			_HighestPeak = highest_peak;
			LOG_INFORMATION("New highest peak encounterd for %s: %f", _Name.Data(), _HighestPeak);
		}

		//Copy the inputs into the outputs.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
		}
	}

private:

	//The name.
	DynamicString _Name;

	//The highest peak.
	float32 _HighestPeak{ 0.0 };

};