#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//STD.
#include <cmath>

//Namespace for all things audio.
namespace Audio
{

	//The default sample rate - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_SAMPLE_RATE{ 48'000.0f };

	//The default beats per minute - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_BEATS_PER_MINUTE{ 120.0f };

	/*
	*	Converts gain to decibels.
	*/
	FORCE_INLINE NO_DISCARD float32 GainToDecibels(const float32 gain) NOEXCEPT
	{
		return 20.0f * std::log10(gain);
	}

	/*
	*	Converts decibels to gain.
	*/
	FORCE_INLINE NO_DISCARD float32 DecibelsToGain(const float32 decibels) NOEXCEPT
	{
		return std::pow(10.0f, decibels * 0.05f);
	}

	/*
	*	Converts time (in seconds) to samples.
	*/
	FORCE_INLINE NO_DISCARD uint32 TimeToSamples(const float32 time, const float32 sample_rate) NOEXCEPT
	{
		return BaseMath::Round<uint32>(time * sample_rate);
	}

}