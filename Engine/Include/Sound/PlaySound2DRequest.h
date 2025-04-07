#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/SoundAsset.h>

//Sound.
#include <Sound/SoundCore.h>

class PlaySound2DRequest final
{

public:

	/*
	*	The sound asset.
	*/
	AssetPointer<SoundAsset> _SoundAsset;

	/*
	*	The gain.
	*	Acts as a simple multiplier to the output sound.
	*/
	float32 _Gain{ 1.0f };

	/*
	*	The pan. 
	*	This is defined as -1.0f being full left, 1.0f being full right and 0.0f being full center.
	*/
	float32 _Pan{ 0.0f };

	/*
	*	The start time.
	*	This can be negative to "queue" a sound to be played in the future,
	*	or positive to skip a segment of the sound
	*	or simply zero to start playing the sound at the start of the sound.
	*/
	float32 _StartTime{ 0.0f };

	/*
	*	Denotes the playback rate.
	*/
	float32 _PlaybackRate{ 1.0f };

	/*
	*	Denotes whether or not the sound is looping.
	*	Looping sounds needs to be explicitly stopped for them to ever stop.
	*	For non-looping sound, they can be played and forgotten about,
	*	and the sound system will automatically clean up it's references to it once it's done.
	*/
	bool _IsLooping{ false };

	/*
	*	Parameters for the ADSR envelope accompanied with every playing sound.
	*/
	float32 _AttackTime{ 0.001f };
	float32 _DecayTime{ 0.001f };
	float32 _SustainGain{ 1.0f };
	float32 _ReleaseTime{ 0.01f };

	/*
	*	The sound instance handle.
	*	This can be nullptr if the requestee does not need to do anything with the sound instance afterwards.
	*/
	SoundInstanceHandle *RESTRICT _SoundInstance{ nullptr };

	/*
	*	The sound started callback.
	*	Will be nullptr in most cases.
	*/
	SoundStartedCallback _SoundStartedCallback{ nullptr };

	/*
	*	The sound stopped callback.
	*	Will be nullptr in most cases.
	*/
	SoundStoppedCallback _SoundStoppedCallback{ nullptr };

	/*
	*	The audio time tracker.
	*	Can be used if playback of sounds need to be tracked with precision.
	*	Will be nullptr in most cases.
	*/
	Atomic<float64> *RESTRICT _AudioTimeTracker{ nullptr };

};