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

//Sound sub system ASIO constants.
namespace SoundSubSystemASIOConstants
{
	//List of desired bit depths.
	constexpr uint8 DESIRED_BIT_DEPTHS[]
	{
		16,
		32,
		8
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
	FORCE_INLINE NO_DISCARD RtAudioFormat GetRtAudioFormat(const uint8 bit_depth) NOEXCEPT
	{
		switch (bit_depth)
		{
			case 8:
			{
				return RTAUDIO_SINT8;
			}

			case 16:
			{
				return RTAUDIO_SINT16;
			}

			case 24:
			{
				return RTAUDIO_SINT24;
			}

			case 32:
			{
				return RTAUDIO_SINT32;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0;
			}
		}
	}

}

/*
*	The error callback.
*/
void ErrorCallback(RtAudioErrorType type, const std::string &error_text) NOEXCEPT
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
*	Queries for available audio devices.
*/
void SoundSubSystemASIO::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Create the Query Rt Audio, if possible.
	if (!_QueryRtAudio)
	{
		_QueryRtAudio = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(RtAudio::Api::WINDOWS_ASIO, ErrorCallback);

		ASSERT(_QueryRtAudio, "Couldn't create query Rt Audio object!");
	}

	//Get the device count.
	const uint32 device_count{ _QueryRtAudio->getDeviceCount() };
	const std::vector<uint32> device_ids{ _QueryRtAudio->getDeviceIds() };

	//Reserve the appropriate size.
	audio_devices->Reserve(device_count);

	//Fill in the audio devices.
	for (uint32 i{ 0 }; i < device_count; ++i)
	{
		//Get the device info.
		const RtAudio::DeviceInfo device_info{ _QueryRtAudio->getDeviceInfo(device_ids[i]) };

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

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT8))
		{
			audio_device._AvailableBitDepths.Emplace(8);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT16))
		{
			audio_device._AvailableBitDepths.Emplace(16);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT24))
		{
			audio_device._AvailableBitDepths.Emplace(24);
		}

		if (TEST_BIT(device_info.nativeFormats, RTAUDIO_SINT32))
		{
			audio_device._AvailableBitDepths.Emplace(32);
		}

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

	ASSERT(!_OpenedAudioDevice._Handle, "Trying to open an audio device when one is already opened, this needs to be handled!");

	//Create the handle.
	_OpenedAudioDevice._Handle = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(RtAudio::Api::WINDOWS_ASIO, ErrorCallback);

	ASSERT(_OpenedAudioDevice._Handle, "Couldn't create query Rt Audio object!");

	//Set the number of channels.
	_NumberOfChannels = 2;

	//Determine the bit depth.
	uint8 bit_depth{ 0 };

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemASIOConstants::DESIRED_BIT_DEPTHS) && bit_depth == 0; ++i)
	{
		for (const uint8 available_bit_depth : _OpenedAudioDevice._AvailableBitDepths)
		{
			if (available_bit_depth == SoundSubSystemASIOConstants::DESIRED_BIT_DEPTHS[i])
			{
				bit_depth = available_bit_depth;

				break;
			}
		}
	}

	ASSERT(bit_depth != 0, "Couldn't find a proper bit depth!");

	//Set the sound format.
	switch (bit_depth)
	{
		case 8:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_8_BIT;

			break;
		}

		case 16:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_16_BIT;

			break;
		}

		case 32:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_32_BIT;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

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
				SoundSubSystemASIOLogic::GetRtAudioFormat(bit_depth),
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
*	Terminates this sound sub system.
*/
void SoundSubSystemASIO::Terminate() NOEXCEPT
{
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

	//Destroy the query RtAudio object.
	static_cast<RtAudio *const RESTRICT>(_QueryRtAudio)->~RtAudio();
	MemorySystem::Instance->TypeFree<RtAudio>(static_cast<RtAudio *const RESTRICT>(_QueryRtAudio));
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
#endif