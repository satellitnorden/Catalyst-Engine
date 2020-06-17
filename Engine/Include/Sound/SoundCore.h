#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using SoundInstanceHandle = uint64;

/*
*	Definition of an empty sound instance handle.
*/
constexpr SoundInstanceHandle EMPTY_SOUND_INSTANCE_HANDLE{ 0 };

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