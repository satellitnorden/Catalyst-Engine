#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Sound/SoundSubSystemWASAPI.h>

#if USE_RT_AUDIO
//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/SoundSystem.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>
#include <Platform/Windows/WindowsUndefines.h>

//Sound sub system WASAPI constants.
namespace SoundSubSystemWASAPIConstants
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

//Sound sub system WASAPI logic.
namespace SoundSubSystemWASAPILogic
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

}

/*
*	The error callback.
*/
static void ErrorCallback(RtAudioErrorType type, const std::string& error_text) NOEXCEPT
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
SoundSubSystemWASAPI::SoundSubSystemWASAPI() NOEXCEPT
{
	//Set the type.
	_Type = SoundSubSystemType::WASAPI;

	//Create the RtAudio object.
	_RtAudio = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(RtAudio::Api::WINDOWS_WASAPI, ErrorCallback);
}

/*
*	Queries for available audio devices.
*/
void SoundSubSystemWASAPI::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
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
void SoundSubSystemWASAPI::Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT
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

		for (const AudioDevice& audio_device : audio_devices)
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

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemWASAPIConstants::DESIRED_SOUND_FORMATS) && _SoundFormat == SoundFormat::UNKNOWN; ++i)
	{
		for (const SoundFormat available_sound_format : _OpenedAudioDevice._AvailableSoundFormats)
		{
			if (available_sound_format == SoundSubSystemWASAPIConstants::DESIRED_SOUND_FORMATS[i])
			{
				_SoundFormat = available_sound_format;

				break;
			}
		}
	}

	ASSERT(_SoundFormat != SoundFormat::UNKNOWN, "Couldn't find a proper sound format!");

	//Determine the sample rate.
	float32 sample_rate{ 0.0f };

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(SoundSubSystemWASAPIConstants::DESIRED_SAMPLE_RATES) && sample_rate == 0.0f; ++i)
	{
		for (const float32 available_sample_rate : _OpenedAudioDevice._AvailableSampleRates)
		{
			if (available_sample_rate == SoundSubSystemWASAPIConstants::DESIRED_SAMPLE_RATES[i])
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

	output_parameters.deviceId = static_cast<RtAudio* const RESTRICT>(_OpenedAudioDevice._Handle)->getDeviceIds()[_OpenedAudioDevice._Index];
	output_parameters.nChannels = _NumberOfChannels;
	output_parameters.firstChannel = 0;

	uint32 buffer_frames{ SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER };

	auto audio_callback{ [](void *outputBuffer, void *inputBuffer,
							unsigned int nFrames,
							double streamTime,
							RtAudioStreamStatus status,
							void *userData)
	{
		const SoundSubSystemWASAPI *const RESTRICT sub_system{ static_cast<const SoundSubSystemWASAPI *const RESTRICT>(userData) };

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
				SoundSubSystemWASAPILogic::GetRtAudioFormat(_SoundFormat),
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
	_SoundSystem->InitializeMixingBuffers(SoundSystem::DEFAULT_NUMBER_OF_MIXING_BUFFERS, SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER);
}

/*
*	Terminates this sound sub system.
*/
void SoundSubSystemWASAPI::Terminate() NOEXCEPT
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
}

/*
*	The update function.
*/
void SoundSubSystemWASAPI::Update() NOEXCEPT
{

}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
NO_DISCARD float32 SoundSubSystemWASAPI::GetAudioLatency() const NOEXCEPT
{
	//Should have practically no latency, no?
	return 0.0f;
}

/*
*	Opens an input stream on the given audio device, with the given parameters.
*/
void SoundSubSystemWASAPI::OpenInputStream
(
	const uint32 start_channel_index,
	const uint32 number_of_channels,
	InputStreamCallback input_stream_callback,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Do nothing here. (:
}
#else
//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/SoundSystem.h>

//Windows.
#include <functiondiscoverykeys_devpkey.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Sound sub system WASAPI constants.
namespace SoundSubSystemWASAPIConstants
{
	constexpr CLSID MMDeviceEnumerator_CLSID{ __uuidof(MMDeviceEnumerator) };
	constexpr IID IAudioClient_IID{ __uuidof(IAudioClient) };
	constexpr IID IAudioRenderClient_IID{ __uuidof(IAudioRenderClient) };
	constexpr IID IMMDeviceEnumerator_IID{ __uuidof(IMMDeviceEnumerator) };

	//List of desired mix formats.
	constexpr WAVEFORMATEX DESIRED_MIX_FORMATS[]
	{
		//Stereo, 44.100 Khz, 16-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 16 / 8,	//nAvgBytesPerSec
			2 * 16 / 8,			//nBlockAlign
			16					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 16-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 16 / 8,	//nAvgBytesPerSec
			2 * 16 / 8,			//nBlockAlign
			16					//wBitsPerSample
		},
		//Stereo, 44.100 Khz, 32-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 32 / 8,	//nAvgBytesPerSec
			2 * 32 / 8,			//nBlockAlign
			32					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 32-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 32 / 8,	//nAvgBytesPerSec
			2 * 32 / 8,			//nBlockAlign
			32					//wBitsPerSample
		},
		//Stereo, 44.100 Khz, 8-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 8 / 8,	//nAvgBytesPerSec
			2 * 8 / 8,			//nBlockAlign
			8					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 8-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 8 / 8,	//nAvgBytesPerSec
			2 * 8 / 8,			//nBlockAlign
			8					//wBitsPerSample
		}
	};
}

//Sound sub system WASAPI logic.
namespace SoundSubSystemWASAPILogic
{

	/*
	*	Calculates the optimal device period.
	*/
	NO_DISCARD REFERENCE_TIME CalculateOptimalDevicePeriod(const DWORD sample_rate) NOEXCEPT
	{
		return static_cast<REFERENCE_TIME>(	/* Seconds */static_cast<float32>(SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) / static_cast<float32>(sample_rate)
											/* Milliseconds */ * 1'000.0f
											/* Nanoseconds */ * 1'000'000.0f
											/* REFERENCE_TIME (100 nanoseconds) */ / 100.0f);
	}

}

/*
*	Queries for available audio devices.
*/
void SoundSubSystemWASAPI::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Define macros.
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { LOG_ERROR("Sound system failted to initialize!, %s failed!", #FUNCTION); goto CLEANUP; }

	//Define local variables.
	IMMDeviceEnumerator *RESTRICT device_enumerator{ nullptr };
	IMMDevice *RESTRICT default_endpoint{ nullptr };
	IMMDeviceCollection *RESTRICT device_collection{ nullptr };
	IMMDevice *RESTRICT endpoint{ nullptr };
	IPropertyStore *RESTRICT property_store{ nullptr };
	PROPVARIANT friendly_name_prop_variant{  };
	DynamicString default_audio_device_name;

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	SoundSubSystemWASAPIConstants::MMDeviceEnumerator_CLSID,
									nullptr,
									CLSCTX_ALL,
									SoundSubSystemWASAPIConstants::IMMDeviceEnumerator_IID,
									reinterpret_cast<void *RESTRICT *const RESTRICT>(&device_enumerator)));

	//Retrieve the default endpoint.
	HANDLE_ERROR(device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
															ERole::eConsole,
															&default_endpoint));

	//Retrieve the name of the default audio device.
	{
		//Open the property store.
		HANDLE_ERROR(default_endpoint->OpenPropertyStore(STGM_READ, &property_store));

		//Initialize the friendly name prop variant.
		PropVariantInit(&friendly_name_prop_variant);

		//Retrieve the name.
		HANDLE_ERROR(property_store->GetValue(PKEY_Device_FriendlyName, &friendly_name_prop_variant));

		std::string endpoint_id_string;

		{
			WCHAR* RESTRICT current = friendly_name_prop_variant.pwszVal;

			while (*current)
			{
				endpoint_id_string += static_cast<char>(*current);
				++current;
			}
		}

		default_audio_device_name = endpoint_id_string.c_str();

		PropVariantClear(&friendly_name_prop_variant);
		SAFE_RELEASE(property_store);
	}

	//Retrieve the audio endpoints.
	HANDLE_ERROR(device_enumerator->EnumAudioEndpoints(	EDataFlow::eRender,
														DEVICE_STATE_ACTIVE,
														&device_collection));

	//Retrieve the number of endpoints.
	uint32 number_of_endpoints{ 0 };
	HANDLE_ERROR(device_collection->GetCount(&number_of_endpoints));

	for (uint32 i{ 0 }; i < number_of_endpoints; ++i)
	{
		//Retrieve the endpoint.
		HANDLE_ERROR(device_collection->Item(i, &endpoint));

		//Add the audio device.
		audio_devices->Emplace();
		AudioDevice &audio_device{ audio_devices->Back() };

		//Set the handle.
		audio_device._Handle = nullptr;

		//Set the index.
		audio_device._Index = i;

		//Open the property store.
		HANDLE_ERROR(endpoint->OpenPropertyStore(STGM_READ, &property_store));

		//Initialize the friendly name prop variant.
		PropVariantInit(&friendly_name_prop_variant);

		//Retrieve the name.
		HANDLE_ERROR(property_store->GetValue(PKEY_Device_FriendlyName, &friendly_name_prop_variant));

		std::string endpoint_id_string;

		{
			WCHAR *RESTRICT current = friendly_name_prop_variant.pwszVal;

			while (*current)
			{
				endpoint_id_string += static_cast<char>(*current);
				++current;
			}
		}

		audio_device._Name = endpoint_id_string.c_str();

		//Set if this is the default audio device.
		audio_device._IsDefault = audio_device._Name == default_audio_device_name;

		PropVariantClear(&friendly_name_prop_variant);
		SAFE_RELEASE(property_store);
		SAFE_RELEASE(endpoint);
	}

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_collection);
	SAFE_RELEASE(default_endpoint);
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}

/*
*	Initializes this sound sub system.
*/
void SoundSubSystemWASAPI::Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT
{
	//Set up the thread.
	_Thread.SetFunctionWithArguments
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<SoundSubSystemWASAPI *const RESTRICT>(arguments)->Update();
		},
		this
	);
	_Thread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_Thread.SetName("Sound Sub System WASAPI Thread");
#endif
	//Launch the thread!
	_Thread.Launch();
}

/*
*	Terminates this sound sub system.
*/
void SoundSubSystemWASAPI::Terminate() NOEXCEPT
{
	//Signal that this sub system should terminate.
	_Terminate = true;

	//Wait for the thread to finish.
	_Thread.Join();

	//Release the audio end point.
	SAFE_RELEASE(_AudioEndpoint);

	//Release the audio client.
	SAFE_RELEASE(_AudioClient);

	//Release the audio render client.
	SAFE_RELEASE(_AudioRenderClient);
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
NO_DISCARD float32 SoundSubSystemWASAPI::GetAudioLatency() const NOEXCEPT
{
	//Ask the audio client for the stream latency.
	REFERENCE_TIME stream_latency;

	if (_AudioClient->GetStreamLatency(&stream_latency) == S_OK)
	{
		//Hard-coded 30 ms adjustment here.
		return static_cast<float32>(stream_latency) / 10'000.0f + 30.0f;
	}

	else
	{
		//Guess?
		return 30.0f;
	}
}

/*
*	The update function.
*/
void SoundSubSystemWASAPI::Update() NOEXCEPT
{
	//Define macros.
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { LOG_ERROR("Sound system failted to initialize!, %s failed!", #FUNCTION); goto CLEANUP; }

	//Define local variables.
	IMMDeviceEnumerator* RESTRICT device_enumerator{ nullptr };
	const WAVEFORMATEX* RESTRICT chosen_mix_format{ nullptr };
	REFERENCE_TIME default_device_period{ 0 };
	REFERENCE_TIME minimum_device_period{ 0 };
	REFERENCE_TIME optimal_device_period{ 0 };
	uint32 current_padding{ 0 };
	uint32 number_of_samples_available{ 0 };
	BYTE *RESTRICT buffer_data{ nullptr };

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	SoundSubSystemWASAPIConstants::MMDeviceEnumerator_CLSID,
									nullptr,
									CLSCTX_ALL,
									SoundSubSystemWASAPIConstants::IMMDeviceEnumerator_IID,
									reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

	//Retrieve the default audio endpoint.
	HANDLE_ERROR(	device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
					ERole::eConsole,
					&_AudioEndpoint));

	//Activated the chosen audio endpoint and retrieve the audio client.
	HANDLE_ERROR(_AudioEndpoint->Activate(	SoundSubSystemWASAPIConstants::IAudioClient_IID,
											CLSCTX_ALL,
											nullptr,
											reinterpret_cast<void *RESTRICT *const RESTRICT>(&_AudioClient)));

	//Check if any of the desired formats are supported.
	for (const WAVEFORMATEX &desired_mix_format : SoundSubSystemWASAPIConstants::DESIRED_MIX_FORMATS)
	{
		WAVEFORMATEX *RESTRICT closest_match_dummy{ nullptr };
		const HRESULT result{ _AudioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &desired_mix_format, &closest_match_dummy) };

		if (closest_match_dummy)
		{
			CoTaskMemFree(closest_match_dummy);
		}

		if (result == S_OK)
		{
			chosen_mix_format = &desired_mix_format;

			break;
		}
	}

	//If none of the desired mix formats were supported, abort.
	if (!chosen_mix_format)
	{
		ASSERT(false, "None of the desired mix formats were supported!");

		LOG_ERROR("Couldn't find a supported mix format!");

		WAVEFORMATEX* RESTRICT device_format{ nullptr };
		HANDLE_ERROR(_AudioClient->GetMixFormat(&device_format));

		if (device_format)
		{
			LOG_INFORMATION("Device format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", device_format->nChannels, device_format->nSamplesPerSec, device_format->wBitsPerSample);

			CoTaskMemFree(device_format);
		}

		goto CLEANUP;
	}

	LOG_INFORMATION("Chosen mix format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", chosen_mix_format->nChannels, chosen_mix_format->nSamplesPerSec, chosen_mix_format->wBitsPerSample);

	//Retrieve the default and minimum device period.
	HANDLE_ERROR(_AudioClient->GetDevicePeriod(&default_device_period, &minimum_device_period));

	//Calculate the optimal device period.
	optimal_device_period = SoundSubSystemWASAPILogic::CalculateOptimalDevicePeriod(chosen_mix_format->nSamplesPerSec);

	//Initialize the audio client.
	HANDLE_ERROR(_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
											0,
											CatalystBaseMath::Maximum<REFERENCE_TIME>(minimum_device_period, optimal_device_period),
											0,
											chosen_mix_format,
											nullptr));

	//Retrieve the buffer size.
	HANDLE_ERROR(_AudioClient->GetBufferSize(&_BufferSize));

	//Retrieve the audio render client.
	HANDLE_ERROR(_AudioClient->GetService(	SoundSubSystemWASAPIConstants::IAudioRenderClient_IID,
											reinterpret_cast<void* RESTRICT* const RESTRICT>(&_AudioRenderClient)));

	//Set the number of channels.
	_NumberOfChannels = static_cast<uint8>(chosen_mix_format->nChannels);

	//Set the sample rate.
	_SampleRate = static_cast<float32>(chosen_mix_format->nSamplesPerSec);

	//Set the sound format.
	switch (chosen_mix_format->wBitsPerSample)
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

	//This sound sub system is successfully initialized!
	_Initialized = true;

	//Initialize the mixing buffers.
	{
		uint32 number_of_mixing_buffers{ SoundSystem::DEFAULT_NUMBER_OF_MIXING_BUFFERS };
		uint32 number_of_samples_per_mixing_buffer{ SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER };

		while ((number_of_mixing_buffers * number_of_samples_per_mixing_buffer) < _BufferSize)
		{
			number_of_mixing_buffers *= 2;
		}

		_SoundSystem->InitializeMixingBuffers(number_of_mixing_buffers, number_of_samples_per_mixing_buffer);
	}

	//Start playing.
	HANDLE_ERROR(_AudioClient->Start());

	//Log.
	LOG_INFORMATION("Sound system successfully initialized!");

	//Main update loop.
	while (!CatalystEngineSystem::Instance->ShouldTerminate() && !_Terminate)
	{
		//Remember the start of the update.
		const std::chrono::steady_clock::time_point start_of_update{ std::chrono::steady_clock::now() };

		//Retrieve the current padding.
		HANDLE_ERROR(_AudioClient->GetCurrentPadding(&current_padding));

		//Calculate the number of samples available.
		number_of_samples_available = _BufferSize - current_padding;

		if (number_of_samples_available > 0)
		{
			//Retrieve the buffer.
			HANDLE_ERROR(_AudioRenderClient->GetBuffer(number_of_samples_available, &buffer_data));

			//Do the update.
			_SoundSystem->SoundCallback(static_cast<float32>(chosen_mix_format->nSamplesPerSec),
										_SoundFormat,
										static_cast<uint8>(chosen_mix_format->nChannels),
										number_of_samples_available,
										buffer_data);

			//Release the buffer.
			HANDLE_ERROR(_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			/*
			//Sleep approximately until the next buffer needs writing.
			const float64 milliseconds_to_sleep{ static_cast<float64>(WindowsSoundSystemData::_BufferSize / 2) / static_cast<float64>(GetSampleRate()) * 1'000.0 };
			const std::chrono::steady_clock::time_point next_update{ start_of_update + std::chrono::nanoseconds(static_cast<uint64>(milliseconds_to_sleep * 1'000'000.0)) };
			std::this_thread::sleep_until(next_update);
			*/
			//Yield the current thread.
			Concurrency::CurrentThread::Yield();
		}
	}

	//Stop playing.
	HANDLE_ERROR(_AudioClient->Stop());

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}

//Undef macros.
#undef SAFE_RELEASE
#endif
#endif