#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Type aliases.
*/
using SoundBankHandle = void *RESTRICT;
using SoundDescriptionHandle = void *RESTRICT;
using SoundInstanceHandle = void *RESTRICT;

//Enumeration covering all playback states.
enum class PlaybackState : uint8
{
	Playing,
	Starting,
	Stopped,
	Stopping,
	Sustaining
};