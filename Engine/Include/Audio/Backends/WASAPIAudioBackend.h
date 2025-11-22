#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/RtAudioAudioBackend.h>

class WASAPIAudioBackend final : public RtAudioAudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE WASAPIAudioBackend(const Parameters &parameters) NOEXCEPT
		:
		RtAudioAudioBackend
		(
			parameters,
			Audio::Backend::WASAPI,
			RtAudio::Api::WINDOWS_WASAPI,
			128
		)
	{

	}

};