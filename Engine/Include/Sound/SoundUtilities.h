#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

//Sound.
#include <Sound/SoundCore.h>

//STL.
#include <cmath>

namespace SoundUtilities
{

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
	*	Converts quality (Q) to bandwidth.
	*/
	FORCE_INLINE NO_DISCARD float32 QualityToBandwidth(const float32 Q) NOEXCEPT
	{
		return std::log(std::sqrt((std::pow((2.0f + (1.0f / std::pow(Q, 2.0f))), 2.0f) / 4.0f) - 1.0f) + (1.0f / (2.0f * std::pow(Q, 2.0f))) + 1.0f) / std::log(2.0f);
	}

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
	*	Calculates the realy note duration in seconds in regards to the beats per minute.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE CalculateNoteDuration(const CombinedNoteDuration note_duration, const TYPE beats_per_minute) NOEXCEPT
	{
		
		switch (note_duration)
		{
			case CombinedNoteDuration::WHOLE_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::WHOLE, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::WHOLE_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::WHOLE, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::WHOLE_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::WHOLE, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::HALF_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::HALF, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::HALF_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::HALF, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::HALF_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::HALF, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::QUARTER_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::QUARTER, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::QUARTER_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::QUARTER, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::QUARTER_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::QUARTER, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::EIGHTH_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::EIGHTH, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::EIGHTH_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::EIGHTH, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::EIGHTH_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::EIGHTH, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTEENTH_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::SIXTEENTH, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTEENTH_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::SIXTEENTH, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTEENTH_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::SIXTEENTH, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::THIRTYSECOND_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::THIRTYSECOND, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::THIRTYSECOND_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::THIRTYSECOND, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::THIRTYSECOND_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::THIRTYSECOND, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTYFOURTH_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::SIXTYFOURTH, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTYFOURTH_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::SIXTYFOURTH, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::SIXTYFOURTH_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::SIXTYFOURTH, NoteType::TRIPLET, beats_per_minute);
			}

			case CombinedNoteDuration::HUNDREDTWENTYEIGHTH_DOTTED:
			{
				return CalculateNoteDuration(NoteDuration::HUNDREDTWENTYEIGHTH, NoteType::DOTTED, beats_per_minute);
			}

			case CombinedNoteDuration::HUNDREDTWENTYEIGHTH_REGULAR:
			{
				return CalculateNoteDuration(NoteDuration::HUNDREDTWENTYEIGHTH, NoteType::REGULAR, beats_per_minute);
			}

			case CombinedNoteDuration::HUNDREDTWENTYEIGHTH_TRIPLET:
			{
				return CalculateNoteDuration(NoteDuration::HUNDREDTWENTYEIGHTH, NoteType::TRIPLET, beats_per_minute);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return static_cast<TYPE>(0);
			}
		}
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

	/*
	*	Calculates a first order low pass alpha.
	*	To be used with lerping between the current and previous sample.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateFirstOrderLowPassAlpha(const float32 frequency, const float32 sample_rate) NOEXCEPT
	{
		const float32 RC{ 1.0f / (2.0f * BaseMathConstants::PI * frequency) };
		const float32 T{ 1.0f / sample_rate };

		return RC / (RC + T);
	}

}