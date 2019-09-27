#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Type aliases.
*/
using SoundBankHandle = void *RESTRICT;
using SoundDescriptionHandle = void *RESTRICT;
using SoundInstanceHandle = void *RESTRICT;

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

//Enumeration covering all playback states.
enum class PlaybackState : uint8
{
	Playing,
	Starting,
	Stopped,
	Stopping,
	Sustaining
};