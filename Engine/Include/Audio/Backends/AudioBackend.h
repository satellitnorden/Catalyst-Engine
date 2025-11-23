#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/AudioDeviceInformation.h>

class AudioBackend
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		//The requested device identifier, if one is requested. Otherwise UINT32_MAXIMUM.
		uint32 _RequestedDeviceIdentifier;

		//The start input channel index.
		uint32 _StartInputChannelIndex;

		//The number of input channels.
		uint32 _NumberOfInputChannels;

		//The start output channel index.
		uint32 _StartOutputChannelIndex;

		//The number of output channels.
		uint32 _NumberOfOutputChannels;

		//The process callback.
		void(*_ProcessCallback)(const DynamicArray<DynamicArray<float32>> &inputs, DynamicArray<DynamicArray<float32>> *const RESTRICT outputs);

	};

	//The backend.
	Audio::Backend _Backend{ Audio::Backend::NONE };

	//The format.
	Audio::Format _Format{ Audio::Format::UNKNOWN };

	//The sample rate.
	float32 _SampleRate{ 0.0f };

	//The buffer size.
	uint32 _BufferSize{ 0 };

	//The number of input channels.
	uint32 _NumberOfInputChannels{ 0 };

	//The number of output channels.
	uint32 _NumberOfOutputChannels{ 0 };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AudioBackend(const Parameters &parameters) NOEXCEPT
		:
		_Parameters(parameters)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioBackend() NOEXCEPT
	{

	}

	/*
	*	Queries audio devices.
	*/
	FORCE_INLINE virtual void QueryAudioDevices(DynamicArray<AudioDeviceInformation> *const RESTRICT audio_devices) NOEXCEPT = 0;

protected:

	//The parameters.
	Parameters _Parameters;

};