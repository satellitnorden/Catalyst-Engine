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
	    const int32 xrun_count = AAudioStream_getXRunCount(stream);

	    if (xrun_count != 0)
        {
	        int x = 0;
        }
#if 1
		static_cast<SoundSystem* const RESTRICT>(user_data)->SoundCallback(	AndroidSoundSystemData::_SampleRate,
																	  		16,
																	  		AndroidSoundSystemData::_NumberOfChannels,
																	  		number_of_samples,
																	  		audio_data);
#endif
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

	//Retrieve the number of channels.
	AndroidSoundSystemData::_NumberOfChannels = AAudioStream_getChannelCount(AndroidSoundSystemData::_AudioStream);

	//Retrieve the sample rate.
	AndroidSoundSystemData::_SampleRate = static_cast<float32>(AAudioStream_getSampleRate(AndroidSoundSystemData::_AudioStream));

	//Set the buffer size.
	AAudioStream_setBufferSizeInFrames(AndroidSoundSystemData::_AudioStream, AAudioStream_getFramesPerBurst(AndroidSoundSystemData::_AudioStream) * 4);

	//Start the stream.
	result = AAudioStream_requestStart(AndroidSoundSystemData::_AudioStream);
	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	//Delete the audio stream builder.
	result = AAudioStreamBuilder_delete(audio_stream_builder);
	ASSERT(result == AAUDIO_OK, AAudio_convertResultToText(result));

	//The Android sound system is successfully initialized!
	AndroidSoundSystemData::_Initialized = true;

	//Initialize the mixing buffers.
	InitializeMixingBuffers(DEFAULT_NUMBER_OF_MIXING_BUFFERS, AAudioStream_getFramesPerBurst(AndroidSoundSystemData::_AudioStream));
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
	//Do nothing.
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