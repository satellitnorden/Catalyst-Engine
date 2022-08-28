#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Systems/SoundSystem.h>

//Android.
#include <aaudio/AAudio.h>

//Android sound system data.
namespace AndroidSoundSystemData
{
	//The audio stream.
	AAudioStream *RESTRICT _AudioStream{ nullptr };

	//The number of channels.
	uint8 _NumberOfChannels;

	//The sample rate.
	float32 _SampleRate;

	//Denotes if the Android sound system is initialized.
	Atomic<bool> _Initialized{ false };
}

/*
*	Queries for audio devices.
*/
void SoundSystem::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Do nothing.
}

/*
*	Opens the given audio device.
*/
void SoundSystem::OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT
{
	//Do nothing.
}

/*
*	Initializes the platform.
*/
void SoundSystem::PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Remember the result.
	aaudio_result_t result;

	//Create the audio stream builder.
	AAudioStreamBuilder *RESTRICT audio_stream_builder{ nullptr };
	result = AAudio_createStreamBuilder(&audio_stream_builder);

	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	if (result != AAUDIO_OK)
	{
        PRINT_TO_OUTPUT("Couldn't initialize AAudio because:" << AAudio_convertResultToText(result));

		return;
	}

	//Set the format.
	AAudioStreamBuilder_setFormat(audio_stream_builder, AAUDIO_FORMAT_PCM_I16);

	//Set the performance mode.
	switch (configuration._SoundSystemMode)
	{
		case SoundSystemMode::DEFAULT:
		{
			AAudioStreamBuilder_setPerformanceMode(audio_stream_builder, AAUDIO_PERFORMANCE_MODE_NONE);

			break;
		}

		case SoundSystemMode::LOW_LATENCY:
		{
			AAudioStreamBuilder_setPerformanceMode(audio_stream_builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the data callback.
	AAudioStreamBuilder_setDataCallback
	(
		audio_stream_builder,
		[](AAudioStream *stream, void *user_data, void *audio_data, int32_t number_of_samples) -> aaudio_data_callback_result_t
		{
			static_cast<SoundSystem *const RESTRICT>(user_data)->SoundCallback(	AndroidSoundSystemData::_SampleRate,
																	  			16,
																	  			AndroidSoundSystemData::_NumberOfChannels,
																	  			number_of_samples,
																	  			audio_data);

			return AAUDIO_CALLBACK_RESULT_CONTINUE;
		},
		this
	);

	//Set the error callback.
	AAudioStreamBuilder_setErrorCallback
	(
		audio_stream_builder,
		[](AAudioStream *stream, void *user_data, aaudio_result_t error)
		{
			ASSERT(false, AAudio_convertResultToText(error));
		},
		nullptr
	);

	//Open the stream.
	result = AAudioStreamBuilder_openStream(audio_stream_builder, &AndroidSoundSystemData::_AudioStream);
	
	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	if (result != AAUDIO_OK)
	{
        PRINT_TO_OUTPUT("Couldn't initialize AAudio because:" << AAudio_convertResultToText(result));

		return;
	}

	//Set the buffer size.
	AAudioStream_setBufferSizeInFrames(AndroidSoundSystemData::_AudioStream, AAudioStream_getFramesPerBurst(AndroidSoundSystemData::_AudioStream) * 2);

	//Start the stream.
	result = AAudioStream_requestStart(AndroidSoundSystemData::_AudioStream);
	
	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	if (result != AAUDIO_OK)
	{
        PRINT_TO_OUTPUT("Couldn't initialize AAudio because:" << AAudio_convertResultToText(result));

		return;
	}

	//Retrieve the number of channels.
	AndroidSoundSystemData::_NumberOfChannels = AAudioStream_getChannelCount(AndroidSoundSystemData::_AudioStream);

    ASSERT(AndroidSoundSystemData::_NumberOfChannels > 0, "Number of channels is invalid!");

	//Retrieve the sample rate.
	AndroidSoundSystemData::_SampleRate = static_cast<float32>(AAudioStream_getSampleRate(AndroidSoundSystemData::_AudioStream));

    ASSERT(AndroidSoundSystemData::_SampleRate > 0, "Sample rate is invalid!");

	//Delete the audio stream builder.
	result = AAudioStreamBuilder_delete(audio_stream_builder);
	
	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	if (result != AAUDIO_OK)
	{
        PRINT_TO_OUTPUT("Couldn't initialize AAudio because:" << AAudio_convertResultToText(result));

		return;
	}

	//The Android sound system is successfully initialized!
	AndroidSoundSystemData::_Initialized = true;

    //Initialize the mixing buffers.
    {
		//Calculate the number of samples per buffer.
		uint32 number_of_samples_per_buffer;

        //Retrieve the number of frames per data callback.
        const uint32 frames_per_data_callback{ static_cast<uint32>(AAudioStream_getFramesPerDataCallback(AndroidSoundSystemData::_AudioStream)) };

		if (frames_per_data_callback == AAUDIO_UNSPECIFIED)
		{
            //Set the number of samples per buffer.
			number_of_samples_per_buffer = DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER;
		}

		else
		{
			//Calculate the number of samples required to fill two data callbacks.
			const uint32 number_of_samples_required_for_data_callbacks{ (frames_per_data_callback * 2) / DEFAULT_NUMBER_OF_MIXING_BUFFERS };

            //Set the number of samples per buffer.
			number_of_samples_per_buffer = CatalystBaseMath::Minimum<uint32>(DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER, number_of_samples_required_for_data_callbacks);
		}

        PRINT_TO_OUTPUT("Number of samples per buffer: " << number_of_samples_per_buffer);

        //Initialize the mixing buffers.
        InitializeMixingBuffers(DEFAULT_NUMBER_OF_MIXING_BUFFERS, number_of_samples_per_buffer);
    }
}

/*
*	Returns if the platform is initialized.
*/
NO_DISCARD bool SoundSystem::PlatformInitialized() const NOEXCEPT
{
	return AndroidSoundSystemData::_Initialized;
}

/*
*	Terminates the platform.
*/
void SoundSystem::PlatformTerminate() NOEXCEPT
{
    //Stop the stream.
    AAudioStream_requestStop(AndroidSoundSystemData::_AudioStream);

	//Close the stream.
	AAudioStream_close(AndroidSoundSystemData::_AudioStream);
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	return AndroidSoundSystemData::_NumberOfChannels;
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
float32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	return AndroidSoundSystemData::_SampleRate;
}

/*
*	Returns the number of bits per sample.
*/
uint8 SoundSystem::GetNumberOfBitsPerSample() const NOEXCEPT
{
	return 16;
}

/*
*	Queries for MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<MIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Do nothing.
}

/*
*	Opens a MIDI device.
*/
void SoundSystem::OpenMIDIDevice(MIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Do nothing.
}

/*
*	Retrieves a MIDI message from the queue of the specified MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(MIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT
{
	//Do nothing.
}

/*
*	The default asynchronous update function.
*/
void SoundSystem::DefaultAsynchronousUpdate() NOEXCEPT
{
	//Do nothing.
}
#endif