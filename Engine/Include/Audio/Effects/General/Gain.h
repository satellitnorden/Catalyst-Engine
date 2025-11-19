#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/SIMD.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

class Gain final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_GAIN{ 1.0f };

	//The gain.
	float32 _Gain{ DEFAULT_GAIN };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Gain(const float32 gain = DEFAULT_GAIN) NOEXCEPT
		:
		_Gain(gain)
	{
		//Make sure the SIMD backend is initialized.
		SIMD::InitializeBackend();
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
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, _Gain);
		}
	}

};