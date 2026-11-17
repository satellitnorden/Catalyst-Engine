#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/RtAudioAudioBackend.h>

class PulseAudioBackend final : public RtAudioAudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PulseAudioBackend(const Parameters &parameters) NOEXCEPT
		:
		RtAudioAudioBackend
		(
			parameters,
			Audio::Backend::PULSE,
			RtAudio::Api::LINUX_PULSE,
			128
		)
	{

	}

};