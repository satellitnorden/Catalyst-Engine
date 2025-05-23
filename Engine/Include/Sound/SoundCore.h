#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all sound formats.
enum class SoundFormat : uint8
{
	SIGNED_INTEGER_8_BIT,
	SIGNED_INTEGER_16_BIT,
	SIGNED_INTEGER_24_BIT,
	SIGNED_INTEGER_32_BIT,
	FLOAT_32_BIT,
	FLOAT_64_BIT,

	UNKNOWN
};

//Enumeration covering all note durations.
enum class NoteDuration : uint8
{
	WHOLE,
	HALF,
	QUARTER,
	EIGHTH,
	SIXTEENTH,
	THIRTYSECOND,
	SIXTYFOURTH,
	HUNDREDTWENTYEIGHTH
};

//Enumeration covering all note types.
enum class NoteType : uint8
{
	REGULAR,
	DOTTED,
	TRIPLET
};

//Enumeration covering all combined note durations. Useful for when effects wants a knob to control the time of something.
enum class CombinedNoteDuration : uint32
{
	WHOLE_DOTTED,
	WHOLE_REGULAR,
	WHOLE_TRIPLET,
	HALF_DOTTED,
	HALF_REGULAR,
	HALF_TRIPLET,
	QUARTER_DOTTED,
	QUARTER_REGULAR,
	QUARTER_TRIPLET,
	EIGHTH_DOTTED,
	EIGHTH_REGULAR,
	EIGHTH_TRIPLET,
	SIXTEENTH_DOTTED,
	SIXTEENTH_REGULAR,
	SIXTEENTH_TRIPLET,
	THIRTYSECOND_DOTTED,
	THIRTYSECOND_REGULAR,
	THIRTYSECOND_TRIPLET,
	SIXTYFOURTH_DOTTED,
	SIXTYFOURTH_REGULAR,
	SIXTYFOURTH_TRIPLET,
	HUNDREDTWENTYEIGHTH_DOTTED,
	HUNDREDTWENTYEIGHTH_REGULAR,
	HUNDREDTWENTYEIGHTH_TRIPLET
};

//List for all names of the combined note durations.
static const char *COMBINED_NOTE_DURATION_NAMES[]
{
	"1/1 D",
	"1/1",
	"1/1 T",

	"1/2 D",
	"1/2",
	"1/2 T",

	"1/4 D",
	"1/4",
	"1/4 T",

	"1/8 D",
	"1/8",
	"1/8 T",

	"1/16 D",
	"1/16",
	"1/16 T",

	"1/32 D",
	"1/32",
	"1/32 T",

	"1/64 D",
	"1/64",
	"1/64 T",

	"1/128 D",
	"1/128",
	"1/128 T"
};

//Sound constants.
namespace SoundConstants
{
	constexpr const char *const RESTRICT MIDI_NOTE_NAMES[]
	{
		"C-1",
		"C#-1",
		"D-1",
		"D#-1",
		"E-1",
		"F-1",
		"F#-1",
		"G-1",
		"G#-1",
		"A-1",
		"A#-1",
		"B-1",

		"C0",
		"C#0",
		"D0",
		"D#0",
		"E0",
		"F0",
		"F#0",
		"G0",
		"G#0",
		"A0",
		"A#0",
		"B0",

		"C1",
		"C#1",
		"D1",
		"D#1",
		"E1",
		"F1",
		"F#1",
		"G1",
		"G#1",
		"A1",
		"A#1",
		"B1",

		"C2",
		"C#2",
		"D2",
		"D#2",
		"E2",
		"F2",
		"F#2",
		"G2",
		"G#2",
		"A2",
		"A#2",
		"B2",

		"C3",
		"C#3",
		"D3",
		"D#3",
		"E3",
		"F3",
		"F#3",
		"G3",
		"G#3",
		"A3",
		"A#3",
		"B3",

		"C4",
		"C#4",
		"D4",
		"D#4",
		"E4",
		"F4",
		"F#4",
		"G4",
		"G#4",
		"A4",
		"A#4",
		"B4",

		"C5",
		"C#5",
		"D5",
		"D#5",
		"E5",
		"F5",
		"F#5",
		"G5",
		"G#5",
		"A5",
		"A#5",
		"B5",

		"C6",
		"C#6",
		"D6",
		"D#6",
		"E6",
		"F6",
		"F#6",
		"G6",
		"G#6",
		"A6",
		"A#6",
		"B6",

		"C7",
		"C#7",
		"D7",
		"D#7",
		"E7",
		"F7",
		"F#7",
		"G7",
		"G#7",
		"A7",
		"A#7",
		"B7",

		"C8",
		"C#8",
		"D8",
		"D#8",
		"E8",
		"F8",
		"F#8",
		"G8",
		"G#8",
		"A8",
		"A#8",
		"B8",

		"C9",
		"C#9",
		"D9",
		"D#9",
		"E9",
		"F9",
		"F#9",
		"G9"
	};
}

//Type aliases.
using SoundInstanceHandle = uint64;
using SoundStartedCallback = void(*)(const SoundInstanceHandle sound_instance);
using SoundStoppedCallback = void(*)(const SoundInstanceHandle sound_instance);
using InputStreamCallback = bool(*)
(
	void *const RESTRICT input_buffer,
	const uint32 number_of_samples,
	const float32 sample_rate,
	const SoundFormat sound_format,
	const uint8 number_of_channels,
	void *const RESTRICT user_data
);

/*
*	Definition of an empty sound instance handle.
*/
constexpr SoundInstanceHandle EMPTY_SOUND_INSTANCE_HANDLE{ 0 };

//Enumeration covering all sound sub system types.
enum class SoundSubSystemType : uint8
{
#if defined(CATALYST_PLATFORM_WINDOWS)
	WASAPI,
	ASIO,
#endif

	DEFAULT //This will pick the default (first) sub system for each platform.
};

/*
*	The audio device picking mode.
*/
enum class AudioDevicePickingMode : uint8
{
	/*
	*	Just picks the default one and rolls with it.
	*/
	PICK_DEFAULT,

	/*
	*	Waits for the user to select one.
	*/
	WAIT_FOR_SELECTION
};

/*
*	Returns the number of bits per sample for the given sound format.
*/
static NO_DISCARD uint8 GetNumberOfBitsPerSample(const SoundFormat sound_format) NOEXCEPT
{
	switch (sound_format)
	{
		case SoundFormat::SIGNED_INTEGER_8_BIT:
		{
			return 8;
		}

		case SoundFormat::SIGNED_INTEGER_16_BIT:
		{
			return 16;
		}

		case SoundFormat::SIGNED_INTEGER_24_BIT:
		{
			return 24;
		}

		case SoundFormat::SIGNED_INTEGER_32_BIT:
		{
			return 32;
		}

		case SoundFormat::FLOAT_32_BIT:
		{
			return 32;
		}

		case SoundFormat::FLOAT_64_BIT:
		{
			return 64;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0;
		}
	}
}