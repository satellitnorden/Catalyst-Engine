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
	FORCE_INLINE static NO_DISCARD TYPE CalculateNoteDuration(const NoteDuration note_duration, const NoteType note_type, const TYPE beats_per_minute) NOEXCEPT
	{
		//Define constants.
		constexpr TYPE DOTTED_MULTIPLIER{ static_cast<TYPE>(1.5) };
		constexpr TYPE TRIPLET_MULTIPLIER{ static_cast<TYPE>(2.0 / 3.0) };

		//Calculate the note duration.
		TYPE calculated_note_duration;

		//Apply the note duration.
		switch (note_duration)
		{
			case NoteDuration::WHOLE:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(4);

				break;
			}

			case NoteDuration::HALF:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(2);

				break;
			}

			case NoteDuration::QUARTER:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute);

				break;
			}

			case NoteDuration::EIGHTH:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.5);

				break;
			}

			case NoteDuration::SIXTEENTH:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.25);

				break;
			}

			case NoteDuration::THIRTYSECOND:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.125);

				break;
			}

			case NoteDuration::SIXTYFOURTH:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.0625);

				break;
			}

			case NoteDuration::HUNDREDTWENTYEIGHTH:
			{
				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.03125);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				calculated_note_duration = (static_cast<TYPE>(60) / beats_per_minute);

				break;
			}
		}

		//Apply the note type.
		switch (note_type)
		{
			case NoteType::REGULAR:
			{
				//Nothing to do here.

				break;
			}

			case NoteType::DOTTED:
			{
				calculated_note_duration *= DOTTED_MULTIPLIER;

				break;
			}

			case NoteType::TRIPLET:
			{
				calculated_note_duration *= TRIPLET_MULTIPLIER;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Return the calculated note duration.
		return calculated_note_duration;
	}

	/*
	*	Performs a crossfade between two sources, and returns the weights for each source.
	*/
	FORCE_INLINE void Crossfade(const float32 alpha, float32 *const RESTRICT A, float32 *const RESTRICT B) NOEXCEPT
	{
		//Define constants.
		constexpr float32 CONSTANT{ 1.4186f };

		//Do the calculation.
		const float32 one_minus_alpha{ 1.0f - alpha };
		const float32 _A{ alpha * one_minus_alpha };
		const float32 _B{ _A * (1.0f + (CONSTANT * _A)) };
		const float32 _C{ _B + alpha };
		const float32 _D{ _B + one_minus_alpha };
		*A = _C * _C;
		*B = _D * _D;
	}

}