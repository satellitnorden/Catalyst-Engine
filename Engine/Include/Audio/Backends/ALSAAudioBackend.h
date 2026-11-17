#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/RtAudioAudioBackend.h>

class ALSAAudioBackend final : public RtAudioAudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ALSAAudioBackend(const Parameters &parameters) NOEXCEPT
		:
		RtAudioAudioBackend
		(
			parameters,
			Audio::Backend::ALSA,
			RtAudio::Api::LINUX_ALSA,
			128
		)
	{

	}

};