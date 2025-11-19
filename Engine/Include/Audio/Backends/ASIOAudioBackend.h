#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Backends/AudioBackend.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>
#include <Platform/Windows/WindowsUndefines.h>

class ASIOAudioBackend final : public AudioBackend
{

public:

	/*
	*	Default constructor.
	*/
	ASIOAudioBackend(const Parameters &parameters) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~ASIOAudioBackend() NOEXCEPT;

private:

	//The RT Audio object.
	RtAudio *RESTRICT _RtAudio{ nullptr };

	//The inputs.
	DynamicArray<DynamicArray<float32>> _Inputs;

	//The outputs.
	DynamicArray<DynamicArray<float32>> _Outputs;

	/*
	*	The audio callback.
	*/
	void AudioCallback(void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status);

};