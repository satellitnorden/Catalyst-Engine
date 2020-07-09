#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Sound.
#include <Sound/SoundCore.h>

namespace SoundUtilities
{

	/*
	*	Calculates the realy note duration in seconds in regards to the beats per minute.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE CalculateNoteDuration(const NoteDuration duration, const TYPE beats_per_minute) NOEXCEPT
	{
		//Calculate the duration for each beat.
		const TYPE beat_duration{ static_cast<TYPE>(60) / beats_per_minute };

		//Modify based on duration.
		switch (duration)
		{
			case NoteDuration::WHOLE:
			{
				return beat_duration * static_cast<TYPE>(4);
			}

			case NoteDuration::HALF:
			{
				return beat_duration * static_cast<TYPE>(2);
			}

			case NoteDuration::QUARTER:
			{
				return beat_duration;
			}

			case NoteDuration::EIGHTH:
			{
				return beat_duration * static_cast<TYPE>(0.5);
			}

			case NoteDuration::SIXTEENTH:
			{
				return beat_duration * static_cast<TYPE>(0.25);
			}

			case NoteDuration::THIRTYSECOND:
			{
				return beat_duration * static_cast<TYPE>(0.125);
			}

			case NoteDuration::SIXTYFOURTH:
			{
				return beat_duration * static_cast<TYPE>(0.0625);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return static_cast<TYPE>(0);
			}
		}
	}

}