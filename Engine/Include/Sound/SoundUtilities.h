#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Sound.
#include <Sound/Native/SoundCore.h>

namespace SoundUtilities
{

	/*
	*	Calculates the realy note duration in seconds in regards to the beats per minute.
	*/
	FORCE_INLINE static NO_DISCARD float CalculateNoteDuration(const NoteDuration duration, const float beats_per_minute) NOEXCEPT
	{
		//Calculate the duration for each beat.
		const float beat_duration{ 60.0f / beats_per_minute };

		//Modify based on duration.
		switch (duration)
		{
			case NoteDuration::WHOLE:
			{
				return beat_duration * 4.0f;
			}

			case NoteDuration::HALF:
			{
				return beat_duration * 2.0f;
			}

			case NoteDuration::QUARTER:
			{
				return beat_duration;
			}

			case NoteDuration::EIGHTH:
			{
				return beat_duration * 0.5f;
			}

			case NoteDuration::SIXTEENTH:
			{
				return beat_duration * 0.25f;
			}

			case NoteDuration::THIRTYSECOND:
			{
				return beat_duration * 0.125f;
			}

			case NoteDuration::SIXTYFOURTH:
			{
				return beat_duration * 0.0625f;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

}