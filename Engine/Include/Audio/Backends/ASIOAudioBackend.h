#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/RtAudioAudioBackend.h>

class ASIOAudioBackend final : public RtAudioAudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ASIOAudioBackend(const Parameters &parameters) NOEXCEPT
		:
		RtAudioAudioBackend
		(
			parameters,
			Audio::Backend::ASIO,
			RtAudio::Api::WINDOWS_ASIO,
			/*
			*	RtAudio documentation is wrong, settings this to 0 will not use the minimum size, but actually the preferred buffer size...
			*	For ASIO users, it's probably expected that we will use whatever buffer size the user sets on their own.
			*/
			0 
		)
	{

	}

};