#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>

//Sound.
#include <Sound/SoundCore.h>

class PlaySoundRequest final
{

public:

	/*
	*	The sound resource.
	*/
	ResourcePointer<SoundResource> _SoundResource;

	/*
	*	The gain.
	*	Acts as a simple multiplier to the output sound.
	*/
	float32 _Gain;

	/*
	*	The pan. 
	*	This is defined as -1.0f being full left, 1.0f being full right and 0.0f being full center.
	*/
	float32 _Pan;

	/*
	*	The start time.
	*	This can be negative to "queue" a sound to be played in the future,
	*	or positive to skip a segment of the sound
	*	or simply zero to start playing the sound at the start of the sound.
	*/
	float32 _StartTime;

	/*
	*	Denotes the playback rate.
	*/
	float32 _PlaybackRate;

	/*
	*	Denotes whether or not the sound is looping.
	*	Looping sounds needs to be explicitly stopped for them to ever stop.
	*	For non-looping sound, they can be played and forgotten about,
	*	and the sound system will automatically clean up it's references to it once it's done.
	*/
	bool _IsLooping;

	/*
	*	Parameters for the ADSR envelope accompanied with every playing sound.
	*/
	float32 _AttackTime;
	float32 _DecayTime;
	float32 _SustainGain;
	float32 _ReleaseTime;

	/*
	*	The sound instance handle.
	*	This can be nullptr if the requestee does not need to do anything with the sound instance afterwards.
	*/
	SoundInstanceHandle* RESTRICT _SoundInstance;

	/*
	*	The sound stopped callback.
	*	Will be nullptr in most cases.
	*/
	SoundStoppedCallback _SoundStoppedCallback;

};