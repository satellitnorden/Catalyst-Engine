#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/AudioBackend.h>
#include <Audio/AudioDeviceInformation.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>
#include <Platform/Windows/WindowsUndefines.h>

class RtAudioAudioBackend : public AudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	RtAudioAudioBackend
	(
		const Parameters &parameters,
		const Audio::Backend backend,
		const RtAudio::Api api,
		const uint32 wanted_buffer_size
	) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~RtAudioAudioBackend() NOEXCEPT;

	/*
	*	Queries audio devices.
	*/
	FORCE_INLINE void QueryAudioDevices(DynamicArray<AudioDeviceInformation> *const RESTRICT audio_devices) NOEXCEPT override
	{
		*audio_devices = _AudioDeviceInformations;
	}

private:

	//The RT Audio object.
	RtAudio *RESTRICT _RtAudio{ nullptr };

	//The audio device informations.
	DynamicArray<AudioDeviceInformation> _AudioDeviceInformations;

	//The inputs.
	DynamicArray<DynamicArray<float32>> _Inputs;

	//The outputs.
	DynamicArray<DynamicArray<float32>> _Outputs;

	/*
	*	The audio callback.
	*/
	void AudioCallback(void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status);

};