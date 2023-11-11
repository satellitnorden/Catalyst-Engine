#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Sound/SoundSubSystemASIO.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/SoundSystem.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>

/*
	*	Opened input stream class definition.
	*/
class OpenedInputStream final
{

public:

	//The parent.
	SoundSubSystemASIO *RESTRICT _Parent;

	//The RtAudio object.
	RtAudio *RESTRICT _RtAudio;

	//The audio device.
	AudioDevice _AudioDevice;

	//The start channel index.
	uint32 _StartChannelIndex;

	//The number of channels.
	uint32 _NumberOfChannels;

	//The input stream callback.
	InputStreamCallback _InputStreamCallback;

	//The user data.
	void *RESTRICT _UserData;

	//The sample rate.
	float32 _SampleRate;

	//The sound format.
	SoundFormat _SoundFormat;

	//The samples buffer.
	DynamicArray<float32> _SamplesBuffer;

	//Denotes if this input stream wants to stop.
	AtomicFlag _WantsToStop;

};

//Sound sub system ASIO constants.
namespace SoundSubSystemASIOConstants
{
	//List of desired sound formats.
	constexpr SoundFormat DESIRED_SOUND_FORMATS[]
	{
		SoundFormat::FLOAT_32_BIT,
		SoundFormat::SIGNED_INTEGER_16_BIT,
		SoundFormat::SIGNED_INTEGER_32_BIT,
		SoundFormat::SIGNED_INTEGER_24_BIT,
		SoundFormat::FLOAT_64_BIT,
		SoundFormat::SIGNED_INTEGER_8_BIT
	};

	//List of desired sample rates.
	constexpr float32 DESIRED_SAMPLE_RATES[]
	{
		44'100.0f,
		48'000.0f
	};
}

//Sound sub system ASIO logic.
namespace SoundSubSystemASIOLogic
{

	/*
	*	Returns the RtAudioFormat for the given bit depth.
	*/
	FORCE_INLINE NO_DISCARD RtAudioFormat GetRtAudioFormat(const SoundFormat sound_format) NOEXCEPT
	{
		switch (sound_format)
		{
			case SoundFormat::SIGNED_INTEGER_8_BIT:
			{
				return RTAUDIO_SINT8;
			}

			case SoundFormat::SIGNED_INTEGER_16_BIT:
			{
				return RTAUDIO_SINT16;
			}

			case SoundFormat::SIGNED_INTEGER_24_BIT:
			{
				return RTAUDIO_SINT24;
			}

			case SoundFormat::SIGNED_INTEGER_32_BIT:
			{
				return RTAUDIO_SINT32;
			}

			case SoundFormat::FLOAT_32_BIT:
			{
				return RTAUDIO_FLOAT32;
			}

			case SoundFormat::FLOAT_64_BIT:
			{
				return RTAUDIO_FLOAT64;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0;
			}
		}
	}

	/*
	*	The input stream callback.
	*/
	FORCE_INLINE NO_DISCARD int32 InputStreamCallback
	(
		void *outputBuffer,
		void *inputBuffer,
		unsigned int nFrames,
		double streamTime,
		RtAudioStreamStatus status,
		void *userData) NOEXCEPT
	{
		//Cache the opened input stream.
		OpenedInputStream *const RESTRICT opened_input_stream{ static_cast<OpenedInputStream* const RESTRICT>(userData) };
	
		//If this input stream wants to stop, do nothing.
		if (opened_input_stream->_WantsToStop.IsSet())
		{
			return 0;
		}

		//Check if we need to upsize the samples buffer.
		if (opened_input_stream->_SamplesBuffer.Size() < nFrames)
		{
			opened_input_stream->_SamplesBuffer.Upsize<false>(nFrames);
		}

		//Convert the incoming buffer to float32.
		for (uint32 sample_index{ 0 }; sample_index < nFrames; ++sample_index)
		{
			switch (opened_input_stream->_SoundFormat)
			{
				case SoundFormat::SIGNED_INTEGER_8_BIT:
				{
					const int8 sample{ static_cast<const int8 *const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = static_cast<float32>(sample) / static_cast<float32>(INT8_MAXIMUM);

					break;
				}

				case SoundFormat::SIGNED_INTEGER_16_BIT:
				{
					const int16 sample{ static_cast<const int16*const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = static_cast<float32>(sample) / static_cast<float32>(INT16_MAXIMUM);

					break;
				}

				/* TODO!
				case SoundFormat::SIGNED_INTEGER_24_BIT:
				{
					const int16 sample{ static_cast<const int16 *const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = static_cast<float32>(sample) / static_cast<float32>(INT16_MAXIMUM);

					break;
				}
				*/

				case SoundFormat::SIGNED_INTEGER_32_BIT:
				{
					const int32 sample{ static_cast<const int32 *const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = static_cast<float32>(sample) / static_cast<float32>(INT32_MAXIMUM);

					break;
				}

				case SoundFormat::FLOAT_32_BIT:
				{
					const float32 sample{ static_cast<const float32 *const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = sample;

					break;
				}

				case SoundFormat::FLOAT_64_BIT:
				{
					const float64 sample{ static_cast<const float64 *const RESTRICT>(inputBuffer)[sample_index] };
					opened_input_stream->_SamplesBuffer[sample_index] = static_cast<float32>(sample);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		//Call the callback.
		const bool wants_to_continue
		{
			opened_input_stream->_InputStreamCallback
			(
				opened_input_stream->_SamplesBuffer.Data(),
				nFrames,
				opened_input_stream->_SampleRate,
				opened_input_stream->_NumberOfChannels,
				opened_input_stream->_UserData
			)
		};

		//Set if this stream wants to stop.
		if (!wants_to_continue)
		{
			opened_input_stream->_WantsToStop.Set();
		}

		return 0;
	}

}

/*
*	The error callback.
*/
static void ErrorCallback(RtAudioErrorType type, const std::string &error_text) NOEXCEPT
{
	switch (type)
	{
		case RtAudioErrorType::RTAUDIO_NO_ERROR:
		{
			ASSERT(false, "This should never happen?");

			break;
		}

		case RtAudioErrorType::RTAUDIO_WARNING:
		{
			LOG_WARNING("Warning: %s", error_text.c_str());

			break;
		}

		case RtAudioErrorType::RTAUDIO_UNKNOWN_ERROR:
		case RtAudioErrorType::RTAUDIO_NO_DEVICES_FOUND:
		case RtAudioErrorType::RTAUDIO_INVALID_DEVICE:
		case RtAudioErrorType::RTAUDIO_DEVICE_DISCONNECT:
		case RtAudioErrorType::RTAUDIO_MEMORY_ERROR:
		case RtAudioErrorType::RTAUDIO_INVALID_PARAMETER:
		case RtAudioErrorType::RTAUDIO_INVALID_USE:
		case RtAudioErrorType::RTAUDIO_DRIVER_ERROR:
		case RtAudioErrorType::RTAUDIO_SYSTEM_ERROR:
		case RtAudioErrorType::RTAUDIO_THREAD_ERROR:
		{
			LOG_ERROR("Error: %s", error_text.c_str());

			break;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			break;
		}
	}
}

/*
*	Default constructor.
*/
SoundSubSystemASIO::SoundSubSystemASIO() NOEXCEPT
{
	//Set the type.
	_Type = SoundSubSystemType::ASIO;

	//Create the RtAudio object.
	_RtAudio = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(RtAudio::Api::WINDOWS_ASIO, ErrorCallback);
}

/*
*	Queries for available audio devices.
*/
void SoundSubSystemASIO::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Get the device count.
	const uint32 device_count{ _RtAudio->getDeviceCount() };
	const std::vector<uint32> device_ids{ _RtAudio->getDeviceIds() };

	//Reserve the appropriate size.
	audio_devices->Reserve(device_count);

	//Fill in the audio devices.
	for (uint32 i{ 0 }; i < device_count; ++i)
	{
		//Get the device info.
		const RtAudio::DeviceInfo device_info{ _RtAudio->getDeviceInfo(device_ids[i]) };

		//Skip if the device isn't valid.
		if (device_info.nativeFormats == 0
			|| device_info.sampleRates.empty())
		{
			continue;
		}

		//Fill in the audio device.
		audio_devices->Emplace();
		AudioDevice &audio_device{ audio_devices->Back() };

		audio_device._Handle = nullptr; //Will be filled in when opening this audio device.
		audio_device._Index = i;
		audio_device._Name = device_info.name.c_str();
		audio_device._IsDefault = device_info.isDefaultOutput;

		ASSERT(device_info.nativeFormats != 0, "Audio device has no sound formats, this shouldn't happen!");

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT8))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::SIGNED_INTEGER_8_BIT);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT16))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::SIGNED_INTEGER_16_BIT);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT24))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::SIGNED_INTEGER_24_BIT);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT32))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::SIGNED_INTEGER_32_BIT);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_FLOAT32))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::FLOAT_32_BIT);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_FLOAT64))
		{
			audio_device._AvailableSoundFormats.Emplace(SoundFormat::FLOAT_64_BIT);
		}

		ASSERT(!audio_device._AvailableSoundFormats.Empty(), "Couldn't gather any available sound formats!");

		audio_device._AvailableSampleRates.Reserve(device_info.sampleRates.size());

		for (const uint32 available_sample_rate : device_info.sampleRates)
		{
			audio_device._AvailableSampleRates.Emplace(static_cast<float32>(available_sample_rate));
		}

		audio_device._PreferredSampleRate = static_cast<float32>(device_info.preferredSampleRate);
	}
}

/*
*	Initializes this sound sub system.
*/
void SoundSubSystemASIO::Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT
{
	//If the audio device isn't set, choose the default one.
	if (initialization_parameters._AudioDevice)
	{
		_OpenedAudioDevice = *initialization_parameters._AudioDevice;
	}

	else
	{
		DynamicArray<AudioDevice> audio_devices;
		QueryAudioDevices(&audio_devices);

		bool found_default{ false };

		for (const AudioDevice &audio_device : audio_devices)
		{
			if (audio_device._IsDefault)
			{
				_OpenedAudioDevice = audio_device;
				found_default = true;

				break;
			}
		}

		if (!found_default)
		{
			//Just pick the first one.
			_OpenedAudioDevice = audio_devices[0];
		}
	}

	//Set the handle
	_OpenedAudioDevice._Handle = _RtAudio;

	//Set the number of channels.
	_NumberOfChannels = 2;

	//Determine the sound format.
	_SoundFormat = SoundFormat::UNKNOWN;

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemASIOConstants::DESIRED_SOUND_FORMATS) && _SoundFormat == SoundFormat::UNKNOWN; ++i)
	{
		for (const SoundFormat available_sound_format : _OpenedAudioDevice._AvailableSoundFormats)
		{
			if (available_sound_format == SoundSubSystemASIOConstants::DESIRED_SOUND_FORMATS[i])
			{
				_SoundFormat = available_sound_format;

				break;
			}
		}
	}

	ASSERT(_SoundFormat != SoundFormat::UNKNOWN, "Couldn't find a proper sound format!");

	//Determine the sample rate.
	float32 sample_rate{ 0.0f };

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemASIOConstants::DESIRED_SAMPLE_RATES) && sample_rate == 0.0f; ++i)
	{
		for (const float32 available_sample_rate : _OpenedAudioDevice._AvailableSampleRates)
		{
			if (available_sample_rate == SoundSubSystemASIOConstants::DESIRED_SAMPLE_RATES[i])
			{
				sample_rate = available_sample_rate;

				break;
			}
		}
	}

	ASSERT(sample_rate != 0.0f, "Couldn't find a proper sample rate!");

	_SampleRate = sample_rate;

	//Open the stream.
	RtAudio::StreamParameters output_parameters;

	output_parameters.deviceId = static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->getDeviceIds()[_OpenedAudioDevice._Index];
	output_parameters.nChannels = _NumberOfChannels;
	output_parameters.firstChannel = 0;

	uint32 buffer_frames{ 0 };

	auto audio_callback{ [](void *outputBuffer, void *inputBuffer,
							unsigned int nFrames,
							double streamTime,
							RtAudioStreamStatus status,
							void *userData)
	{
		const SoundSubSystemASIO *const RESTRICT sub_system{static_cast<const SoundSubSystemASIO *const RESTRICT>(userData)};

		sub_system->_SoundSystem->SoundCallback
		(
			sub_system->_SampleRate,
			sub_system->_SoundFormat,
			sub_system->_NumberOfChannels,
			nFrames,
			outputBuffer
		);

		return 0;
	} };



	RtAudio::StreamOptions stream_options;

	stream_options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
	stream_options.numberOfBuffers = 0;
	stream_options.streamName = "";
	stream_options.priority = 0;

	{
		const RtAudioErrorType error_type
		{
			static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->openStream
			(
				&output_parameters,
				nullptr,
				SoundSubSystemASIOLogic::GetRtAudioFormat(_SoundFormat),
				static_cast<uint32>(sample_rate),
				&buffer_frames,
				audio_callback,
				this,
				&stream_options
			)
		};

		ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
	}

	{
		const RtAudioErrorType error_type
		{
			static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->startStream()
		};

		ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
	}

	//Set the buffer size.
	_BufferSize = buffer_frames;

	//This sound sub system is successfully initialized!
	_Initialized = true;

	//Initialize the mixing buffers.
	_SoundSystem->InitializeMixingBuffers(stream_options.numberOfBuffers + 2 /* 1 extra to avoid stutters. (: */, buffer_frames);
}

/*
*	Updates this sound sub system from the mixing thread.
*/
void SoundSubSystemASIO::MixUpdate() NOEXCEPT
{
	//Check if any opened input streams wants to stop.
	for (uint64 i{ 0 }; i < _OpenedInputStreams.Size();)
	{
		OpenedInputStream *const RESTRICT opened_input_stream{ _OpenedInputStreams[i] };

		if (opened_input_stream->_WantsToStop.IsSet())
		{
			//Stop the stream.
			{
				const RtAudioErrorType error_type
				{
					opened_input_stream->_RtAudio->stopStream()
				};

				ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
			}

			//Close the stream.
			opened_input_stream->_RtAudio->closeStream();

			//Destroy the RtAudio object.
			opened_input_stream->_RtAudio->~RtAudio();
			MemorySystem::Instance->TypeFree<RtAudio>(opened_input_stream->_RtAudio);

			//Destroy the opened input stream object.
			opened_input_stream->~OpenedInputStream();
			Memory::Free(opened_input_stream);

			//Erase the opened input stream.
			_OpenedInputStreams.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Terminates this sound sub system.
*/
void SoundSubSystemASIO::Terminate() NOEXCEPT
{
	//Shut down all opened input stream.
	for (OpenedInputStream *const RESTRICT opened_input_stream : _OpenedInputStreams)
	{
		//Stop the stream.
		{
			const RtAudioErrorType error_type
			{
				opened_input_stream->_RtAudio->stopStream()
			};

			ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
		}

		//Close the stream.
		opened_input_stream->_RtAudio->closeStream();

		//Destroy the RtAudio object.
		opened_input_stream->_RtAudio->~RtAudio();
		MemorySystem::Instance->TypeFree<RtAudio>(opened_input_stream->_RtAudio);

		//Destroy the opened input stream object.
		opened_input_stream->~OpenedInputStream();
		Memory::Free(opened_input_stream);
	}

	_OpenedInputStreams.Clear();

	//Stop the stream.
	{
		const RtAudioErrorType error_type
		{
			static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->stopStream()
		};

		ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
	}

	//Close the stream.
	static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->closeStream();

	//Destroy the RtAudio object.
	static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle)->~RtAudio();
	MemorySystem::Instance->TypeFree<RtAudio>(static_cast<RtAudio *const RESTRICT>(_OpenedAudioDevice._Handle));
}

/*
*	The update function.
*/
void SoundSubSystemASIO::Update() NOEXCEPT
{
	
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
NO_DISCARD float32 SoundSubSystemASIO::GetAudioLatency() const NOEXCEPT
{
	//Should have practically no latency, no?
	return 0.0f;
}

/*
*	Opens an input stream on the given audio device, with the given parameters.
*/
void SoundSubSystemASIO::OpenInputStream
(
	AudioDevice *const RESTRICT audio_device,
	const uint32 start_channel_index,
	const uint32 number_of_channels,
	InputStreamCallback input_stream_callback,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Allocate the new opened input stream object.
	OpenedInputStream *const RESTRICT new_opened_input_stream{ new (Memory::Allocate(sizeof(OpenedInputStream))) OpenedInputStream() };
	Memory::Set(new_opened_input_stream, 0, sizeof(OpenedInputStream));

	//Set the parent.
	new_opened_input_stream->_Parent = this;

	//Allocate the RtAudio object.
	new_opened_input_stream->_RtAudio = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(RtAudio::Api::WINDOWS_ASIO, ErrorCallback);

	//Set the audio device.
	new_opened_input_stream->_AudioDevice = *audio_device;

	//Set the start channel index.
	new_opened_input_stream->_StartChannelIndex = start_channel_index;

	//Set the number of channels.
	new_opened_input_stream->_NumberOfChannels = number_of_channels;

	//Set the input stream callback.
	new_opened_input_stream->_InputStreamCallback = input_stream_callback;

	//The the user data.
	new_opened_input_stream->_UserData = user_data;

	//Figure out the sample rate.
	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemASIOConstants::DESIRED_SAMPLE_RATES) && new_opened_input_stream->_SampleRate == 0.0f; ++i)
	{
		for (const float32 available_sample_rate : _OpenedAudioDevice._AvailableSampleRates)
		{
			if (available_sample_rate == SoundSubSystemASIOConstants::DESIRED_SAMPLE_RATES[i])
			{
				new_opened_input_stream->_SampleRate = available_sample_rate;

				break;
			}
		}
	}

	ASSERT(new_opened_input_stream->_SampleRate != 0.0f, "Couldn't find a proper sample rate!");

	//Figure out the sound format.
	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemASIOConstants::DESIRED_SOUND_FORMATS) && new_opened_input_stream->_SoundFormat == SoundFormat::UNKNOWN; ++i)
	{
		for (const SoundFormat available_sound_format : _OpenedAudioDevice._AvailableSoundFormats)
		{
			if (available_sound_format == SoundSubSystemASIOConstants::DESIRED_SOUND_FORMATS[i])
			{
				new_opened_input_stream->_SoundFormat = available_sound_format;

				break;
			}
		}
	}

	ASSERT(new_opened_input_stream->_SoundFormat != SoundFormat::UNKNOWN, "Couldn't find a proper sound format!");

	//Open the stream.
	RtAudio::StreamParameters input_parameters;

	input_parameters.deviceId = new_opened_input_stream->_RtAudio->getDeviceIds()[_OpenedAudioDevice._Index];
	input_parameters.nChannels = new_opened_input_stream->_NumberOfChannels;
	input_parameters.firstChannel = new_opened_input_stream->_StartChannelIndex;

	uint32 buffer_frames{ 0 };

	RtAudio::StreamOptions stream_options;

	stream_options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
	stream_options.numberOfBuffers = 0;
	stream_options.streamName = "";
	stream_options.priority = 0;

	{
		const RtAudioErrorType error_type
		{
			new_opened_input_stream->_RtAudio->openStream
			(
				nullptr,
				&input_parameters,
				SoundSubSystemASIOLogic::GetRtAudioFormat(new_opened_input_stream->_SoundFormat),
				static_cast<uint32>(new_opened_input_stream->_SampleRate),
				&buffer_frames,
				SoundSubSystemASIOLogic::InputStreamCallback,
				new_opened_input_stream,
				&stream_options
			)
		};

		ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
	}

	{
		const RtAudioErrorType error_type
		{
			new_opened_input_stream->_RtAudio->startStream()
		};

		ASSERT(error_type == RtAudioErrorType::RTAUDIO_NO_ERROR, "Error!");
	}

	//Add the new opened input stream.
	_OpenedInputStreams.Emplace(new_opened_input_stream);
}
#endif