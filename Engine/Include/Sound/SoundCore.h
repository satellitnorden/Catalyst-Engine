#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using SoundInstanceHandle = uint64;

/*
*	Definition of an empty sound instance handle.
*/
constexpr SoundInstanceHandle EMPTY_SOUND_INSTANCE_HANDLE{ 0 };

//Enumeration covering all sound system modes.
enum class SoundSystemMode : uint8
{
	/*
	*	Will use the default platform API for processing sound.
	*/
	DEFAULT,

	/*
	*	Will use a low latency platform API for processing sound, if one exists.
	*/
	LOW_LATENCY
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
	SIXTYFOURTH
};

//Enumeration covering all note types.
enum class NoteType : uint8
{
	REGULAR,
	DOTTED,
	TRIPLET
};