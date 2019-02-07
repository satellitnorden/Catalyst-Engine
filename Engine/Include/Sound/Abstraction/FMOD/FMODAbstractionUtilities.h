#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Sound.
#include <Sound/Abstraction/FMOD/FMODCore.h>
#include <Sound/Native/SoundCore.h>

namespace FMODAbstractionUtilities
{

	/*
	*	Converts the FMOD specific FMOD_STUDIO_PLAYBACK_STATE enumeration to the native PlaybackState enumeration.
	*/
	static NO_DISCARD PlaybackState GetPlaybackState(const FMOD_STUDIO_PLAYBACK_STATE state) NOEXCEPT
	{
		switch (state)
		{
			case FMOD_STUDIO_PLAYBACK_PLAYING:
			{
				return PlaybackState::Playing;
			}

			case FMOD_STUDIO_PLAYBACK_SUSTAINING:
			{
				return PlaybackState::Sustaining;
			}

			case FMOD_STUDIO_PLAYBACK_STOPPED:
			{
				return PlaybackState::Stopped;
			}

			case FMOD_STUDIO_PLAYBACK_STARTING:
			{
				return PlaybackState::Starting;
			}

			case FMOD_STUDIO_PLAYBACK_STOPPING:
			{
				return PlaybackState::Stopping;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return PlaybackState::Stopped;
			}
		}
	}
}