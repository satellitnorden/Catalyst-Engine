#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>
#include <ThirdParty/RtMidi/RtMidi.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Windows sound system constants.
namespace WindowsSoundSystemConstants
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

//Windows sound system data.
namespace WindowsSoundSystemData
{

	//The Rt Audio API.
	RtAudio::Api _RtAudioAPI;

	//The query Rt Audio.
	RtAudio *RESTRICT _QueryRtAudio{ nullptr };

	//The opened Rt Audio.
	RtAudio *RESTRICT _OpenedRtAudio{ nullptr };

	//Denotes if the Windows sound system is initialized.
	Atomic<bool> _Initialized{ false };

	//The number of channels.
	uint8 _NumberOfChannels;

	//The sample rate.
	float32 _SampleRate;

	//The sound format.
	SoundFormat _SoundFormat{ SoundFormat::UNKNOWN };

	//The query MIDI in.
	RtMidiIn *RESTRICT _QueryMIDIIn{ nullptr };

	//The opened MIDI ins.
	DynamicArray<RtMidiIn *RESTRICT> _OpenedMIDIIns;

	//The thread.
	Thread _Thread;

	//The audio endpoint.
	IMMDevice *RESTRICT _AudioEndpoint{ nullptr };

	//The audio client.
	IAudioClient *RESTRICT _AudioClient{ nullptr };

	//The audio render client.
	IAudioRenderClient *RESTRICT _AudioRenderClient{ nullptr };

}

//Windows sound system logic.
namespace WindowsSoundSystemLogic
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
*	Queries for audio devices.
*/
void SoundSystem::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Create the Query Rt Audio, if possible.
	if (!WindowsSoundSystemData::_QueryRtAudio)
	{
		try
		{
			WindowsSoundSystemData::_QueryRtAudio = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(WindowsSoundSystemData::_RtAudioAPI);
		}

		catch (RtAudioError &error_message)
		{
			ASSERT(false, error_message.getMessage());
		}

		ASSERT(WindowsSoundSystemData::_QueryRtAudio, "Couldn't create query Rt Audio object!");
	}

	//Get the device count.
	const uint32 device_count{ WindowsSoundSystemData::_QueryRtAudio->getDeviceCount() };

	//Reserve the appropriate size.
	audio_devices->Reserve(device_count);

	//Fill in the audio devices.
	for (uint32 i{ 0 }; i < device_count; ++i)
	{
		//Get the device info.
		const RtAudio::DeviceInfo device_info{ WindowsSoundSystemData::_QueryRtAudio->getDeviceInfo(i) };

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
*	Opens the given audio device.
*/
void SoundSystem::OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT
{
	ASSERT(!WindowsSoundSystemData::_OpenedRtAudio, "Trying to open an audio device when one is already opened, this needs to be handled!");

	//Create the handle.
	try
	{
		audio_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtAudio>()) RtAudio(WindowsSoundSystemData::_RtAudioAPI);
	}

	catch (RtAudioError &error_message)
	{
		ASSERT(false, error_message.getMessage());
	}

	ASSERT(audio_device->_Handle, "Couldn't create query Rt Audio object!");

	//Set the number of channels.
	WindowsSoundSystemData::_NumberOfChannels = 2;

	//Determine the bit depth.
	uint8 bit_depth{ 0 };

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(WindowsSoundSystemConstants::DESIRED_BIT_DEPTHS); ++i)
	{
		for (const uint8 available_bit_depth : audio_device->_AvailableBitDepths)
		{
			if (available_bit_depth == WindowsSoundSystemConstants::DESIRED_BIT_DEPTHS[i])
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
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_8_BIT;

			break;
		}

		case 16:
		{
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_16_BIT;

			break;
		}

		case 32:
		{
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_32_BIT;

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

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(WindowsSoundSystemConstants::DESIRED_SAMPLE_RATES); ++i)
	{
		for (const float32 available_sample_rate : audio_device->_AvailableSampleRates)
		{
			if (available_sample_rate == WindowsSoundSystemConstants::DESIRED_SAMPLE_RATES[i])
			{
				sample_rate = available_sample_rate;

				break;
			}
		}
	}

	ASSERT(sample_rate != 0.0f, "Couldn't find a proper sample rate!");

	WindowsSoundSystemData::_SampleRate = sample_rate;

	//Open the stream.
	RtAudio::StreamParameters output_parameters;

	output_parameters.deviceId = audio_device->_Index;
	output_parameters.nChannels = 2;
	output_parameters.firstChannel = 0;

	uint32 buffer_frames{ 0 };

	auto audio_callback{ [](void *outputBuffer, void *inputBuffer,
							unsigned int nFrames,
							double streamTime,
							RtAudioStreamStatus status,
							void *userData)
	{
		static_cast<SoundSystem* const RESTRICT>(userData)->SoundCallback(	WindowsSoundSystemData::_SampleRate,
																			WindowsSoundSystemData::_SoundFormat,
																			WindowsSoundSystemData::_NumberOfChannels,
																			nFrames,
																			outputBuffer);

		return 0;
	} };

	RtAudio::StreamOptions stream_options;

	stream_options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
	stream_options.numberOfBuffers = 0;
	stream_options.streamName = "";
	stream_options.priority = 0;
	
	try
	{
		static_cast<RtAudio *const RESTRICT>(audio_device->_Handle)->openStream(&output_parameters,
																				nullptr,
																				WindowsSoundSystemLogic::GetRtAudioFormat(bit_depth),
																				static_cast<uint32>(sample_rate),
																				&buffer_frames,
																				audio_callback,
																				this,
																				&stream_options,
																				nullptr);
	}

	catch (RtAudioError &error_message)
	{
		ASSERT(false, error_message.getMessage());
	}

	//Start the stream.
	try
	{
		static_cast<RtAudio *const RESTRICT>(audio_device->_Handle)->startStream();
	}

	catch (RtAudioError &error_message)
	{
		ASSERT(false, error_message.getMessage());
	}

	//The Windows sound system is successfully initialized!
	WindowsSoundSystemData::_Initialized = true;

	//Initialize the mixing buffers.
	InitializeMixingBuffers(stream_options.numberOfBuffers, buffer_frames);

	//Set the opened Rt Audio.
	WindowsSoundSystemData::_OpenedRtAudio = static_cast<RtAudio* const RESTRICT>(audio_device->_Handle);
}

/*
*	Initializes the platform.
*/
void SoundSystem::PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set the Rt Audio API.
	switch (configuration._SoundSystemMode)
	{
		case SoundSystemMode::DEFAULT:
		{
			WindowsSoundSystemData::_RtAudioAPI = RtAudio::Api::WINDOWS_DS;

			break;
		}

		case SoundSystemMode::LOW_LATENCY:
		{
			WindowsSoundSystemData::_RtAudioAPI = RtAudio::Api::WINDOWS_ASIO;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Is this the default sound system mode?
	if (configuration._SoundSystemMode == SoundSystemMode::DEFAULT)
	{
		//Set up the thread.
		WindowsSoundSystemData::_Thread.SetFunction([]()
		{
			SoundSystem::Instance->DefaultAsynchronousUpdate();
		});
		WindowsSoundSystemData::_Thread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
		WindowsSoundSystemData::_Thread.SetName("Sound System - Platform Thread");
#endif
		//Launch the thread!
		WindowsSoundSystemData::_Thread.Launch();
	}

	//Is this the low latency sound system mode?
	else if (configuration._SoundSystemMode == SoundSystemMode::LOW_LATENCY)
	{
		//Should the default sound device be picked?
		if (configuration._AudioDevicePickingMode == AudioDevicePickingMode::PICK_DEFAULT)
		{
			//Query the audio devices.
			DynamicArray<AudioDevice> audio_devices;

			QueryAudioDevices(&audio_devices);

#if 1
			//Open the default audio device.
			for (AudioDevice &audio_device : audio_devices)
			{
				if (audio_device._IsDefault)
				{
					OpenAudioDevice(&audio_device);
				}
			}
#else
			bool opened_audio_device{ false };

			for (AudioDevice &audio_device : audio_devices)
			{
				if (audio_device._Name.Find("Focusrite") != nullptr)
				{
					OpenAudioDevice(&audio_device);

					opened_audio_device = true;

					break;
				}
			}

			if (!opened_audio_device)
			{
				OpenAudioDevice(&audio_devices[1]);
			}
#endif
		}
	}
}

/*
*	Returns if the platform is initialized.
*/
NO_DISCARD bool SoundSystem::PlatformInitialized() const NOEXCEPT
{
	return WindowsSoundSystemData::_Initialized;
}

/*
*	Terminates the platform.
*/
void SoundSystem::PlatformTerminate() NOEXCEPT
{
	//Destroy the query Rt Midi In.
	if (WindowsSoundSystemData::_QueryMIDIIn)
	{
		WindowsSoundSystemData::_QueryMIDIIn->~RtMidiIn(); //The MemorySystem will take care of actually freeing the memory. (:
	}

	//Destroy the opened MIDI ins.
	for (RtMidiIn *const RESTRICT opened_midi_in : WindowsSoundSystemData::_OpenedMIDIIns)
	{
		opened_midi_in->closePort();
		opened_midi_in->~RtMidiIn(); //The MemorySystem will take care of actually freeing the memory. (:
	}

	switch (WindowsSoundSystemData::_RtAudioAPI)
	{
		case RtAudio::Api::WINDOWS_ASIO:
		{
			//Close the opened Rt Audio.
			if (WindowsSoundSystemData::_OpenedRtAudio)
			{
				WindowsSoundSystemData::_OpenedRtAudio->stopStream();
				WindowsSoundSystemData::_OpenedRtAudio->closeStream();
				WindowsSoundSystemData::_OpenedRtAudio->~RtAudio(); //The MemorySystem will take care of actually freeing the memory. (:
			}

			//Destroy the query Rt Audio.
			if (WindowsSoundSystemData::_QueryRtAudio)
			{
				WindowsSoundSystemData::_QueryRtAudio->~RtAudio(); //The MemorySystem will take care of actually freeing the memory. (:
			}

			break;
		}

		case RtAudio::Api::WINDOWS_DS:
		{
			//Wait for the thread to finish.
			WindowsSoundSystemData::_Thread.Join();

			//Release the audio end point.
			SAFE_RELEASE(WindowsSoundSystemData::_AudioEndpoint);

			//Release the audio client.
			SAFE_RELEASE(WindowsSoundSystemData::_AudioClient);

			//Release the audio render client.
			SAFE_RELEASE(WindowsSoundSystemData::_AudioRenderClient);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	//Don't do anything if the Windows sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	ASSERT(WindowsSoundSystemData::_NumberOfChannels != 0, "Oh no!");

	return WindowsSoundSystemData::_NumberOfChannels;
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
float32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	//Don't do anything if the Windows sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	ASSERT(WindowsSoundSystemData::_SampleRate != 0.0f, "Oh no!");

	return WindowsSoundSystemData::_SampleRate;
}

/*
*	Returns the sound format.
*/
SoundFormat SoundSystem::GetSoundFormat() const NOEXCEPT
{
	//Don't do anything if the Windows sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return SoundFormat::UNKNOWN;
	}

	ASSERT(WindowsSoundSystemData::_SoundFormat != SoundFormat::UNKNOWN, "Oh no!");

	return WindowsSoundSystemData::_SoundFormat;
}

/*
*	Queries for MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<MIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Create the query MIDI in, if necessary.
	if (!WindowsSoundSystemData::_QueryMIDIIn)
	{
		WindowsSoundSystemData::_QueryMIDIIn = new (MemorySystem::Instance->TypeAllocate<RtMidiIn>()) RtMidiIn();

		ASSERT(WindowsSoundSystemData::_QueryMIDIIn, "Couldn't create MIDI in!");
	}

	//Query the port count.
	const uint32 port_count{ WindowsSoundSystemData::_QueryMIDIIn->getPortCount() };

	//Reserve the appropriate amount.
	midi_devices->Reserve(port_count);

	//Fill in the midi devices.
	for (uint32 i{ 0 }; i < port_count; ++i)
	{
		midi_devices->Emplace();
		MIDIDevice &midi_device{ midi_devices->Back() };

		midi_device._Handle = nullptr;
		midi_device._Index = i;
		midi_device._Name = WindowsSoundSystemData::_QueryMIDIIn->getPortName(i).c_str();
	}
}

/*
*	Opens a MIDI device.
*/
void SoundSystem::OpenMIDIDevice(MIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Create the MIDI in.
	midi_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtMidiIn>()) RtMidiIn();

	//Open the port.
	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->openPort(midi_device->_Index);

	//Add to the opened MIDI ins.
	WindowsSoundSystemData::_OpenedMIDIIns.Emplace(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));
}

/*
*	Retrieves a MIDI message from the queue of the specified MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(MIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT
{
	//Get the message.
	static thread_local std::vector<uint8> message;
	message.clear();

	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->getMessage(&message);

	//Was a message received?
	if (message.empty())
	{
		return false;
	}

	else
	{
		//Is this a note on?
		if (message.size() == 3 && ((message[0] & 0xf0) == 0x90) && message[2] != 0)
		{
			midi_message->_Type = MIDIMessage::Type::NOTE_ON;
			midi_message->_NoteOnNote = message[1];
			midi_message->_NoteOnVelocity = message[2];
		}

		//Is this an aftertouch?
		else if (message.size() == 3 && ((message[0] & 0xf0) == 0xA0))
		{
			midi_message->_Type = MIDIMessage::Type::AFTERTOUCH;
			midi_message->_AftertouchNote = message[1];
		}

		//The MIDI message wasn't a valid type.
		else
		{
			return false;
		}

		//Return that the retrieval succeeded.
		return true;
	}
}

/*
*	The default asynchronous update function.
*/
void SoundSystem::DefaultAsynchronousUpdate() NOEXCEPT
{
	//Define macros.
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { LOG_ERROR("Sound system failted to initialize!, %s failed!", #FUNCTION); goto CLEANUP; }

	//Define local variables.
	IMMDeviceEnumerator* RESTRICT device_enumerator{ nullptr };
	const WAVEFORMATEX *RESTRICT chosen_mix_format{ nullptr };
	uint32 buffer_size{ 0 };
	REFERENCE_TIME default_device_period{ 0 };
	REFERENCE_TIME minimum_device_period{ 0 };
	REFERENCE_TIME optimal_device_period{ 0 };
	uint32 current_padding{ 0 };
	uint32 number_of_samples_available{ 0 };
	BYTE *RESTRICT buffer_data{ nullptr };

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	WindowsSoundSystemConstants::MMDeviceEnumerator_CLSID,
									nullptr,
									CLSCTX_ALL,
									WindowsSoundSystemConstants::IMMDeviceEnumerator_IID,
									reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

	//Retrieve the default audio endpoint.
	HANDLE_ERROR(device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
															ERole::eConsole,
															&WindowsSoundSystemData::_AudioEndpoint));

	//Activated the chosen audio endpoint and retrieve the audio client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioEndpoint->Activate(	WindowsSoundSystemConstants::IAudioClient_IID,
																	CLSCTX_ALL,
																	nullptr,
																	reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsSoundSystemData::_AudioClient)));

	//Check if any of the desired formats are supported.
	for (const WAVEFORMATEX &desired_mix_format : WindowsSoundSystemConstants::DESIRED_MIX_FORMATS)
	{
		WAVEFORMATEX *RESTRICT closest_match_dummy{ nullptr };
		const HRESULT result{ WindowsSoundSystemData::_AudioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &desired_mix_format, &closest_match_dummy) };

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

		WAVEFORMATEX *RESTRICT device_format{ nullptr };
		HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetMixFormat(&device_format));

		if (device_format)
		{
			LOG_INFORMATION("Device format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", device_format->nChannels, device_format->nSamplesPerSec, device_format->wBitsPerSample);
		
			CoTaskMemFree(device_format);
		}

		goto CLEANUP;
	}

	LOG_INFORMATION("Chosen mix format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", chosen_mix_format->nChannels, chosen_mix_format->nSamplesPerSec, chosen_mix_format->wBitsPerSample);

	//Retrieve the default and minimum device period.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetDevicePeriod(&default_device_period, &minimum_device_period));

	//Calculate the optimal device period.
	optimal_device_period = WindowsSoundSystemLogic::CalculateOptimalDevicePeriod(chosen_mix_format->nSamplesPerSec);

	//Initialize the audio client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
																	0,
																	CatalystBaseMath::Maximum<REFERENCE_TIME>(minimum_device_period, optimal_device_period),
																	0,
																	chosen_mix_format,
																	nullptr));

	//Retrieve the buffer size.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetBufferSize(&buffer_size));

	//Retrieve the audio render client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetService(	WindowsSoundSystemConstants::IAudioRenderClient_IID,
																	reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsSoundSystemData::_AudioRenderClient)));

	//Set the number of channels.
	WindowsSoundSystemData::_NumberOfChannels = static_cast<uint8>(chosen_mix_format->nChannels);

	//Set the sample rate.
	WindowsSoundSystemData::_SampleRate = static_cast<float32>(chosen_mix_format->nSamplesPerSec);

	//Set the sound format.
	switch (chosen_mix_format->wBitsPerSample)
	{
		case 8:
		{
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_8_BIT;

			break;
		}

		case 16:
		{
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_16_BIT;

			break;
		}

		case 32:
		{
			WindowsSoundSystemData::_SoundFormat = SoundFormat::SIGNED_INTEGER_32_BIT;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//The Windows sound system is successfully initialized!
	WindowsSoundSystemData::_Initialized = true;

	//Initialize the mixing buffers.
	InitializeMixingBuffers(DEFAULT_NUMBER_OF_MIXING_BUFFERS, DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER);

	//Start playing.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Start());

	//Log.
	LOG_INFORMATION("Sound system successfully initialized!");

	//Main update loop.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Retrieve the current padding.
		HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetCurrentPadding(&current_padding));

		//Calculate the number of samples available.
		number_of_samples_available = buffer_size - current_padding;

		if (number_of_samples_available > 0)
		{
			//Retrieve the buffer.
			HANDLE_ERROR(WindowsSoundSystemData::_AudioRenderClient->GetBuffer(number_of_samples_available, &buffer_data));

			//Do the update.
			SoundCallback(	static_cast<float32>(chosen_mix_format->nSamplesPerSec),
							WindowsSoundSystemData::_SoundFormat,
							static_cast<uint8>(chosen_mix_format->nChannels),
							number_of_samples_available,
							buffer_data);

			//Release the buffer.
			HANDLE_ERROR(WindowsSoundSystemData::_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			//Yield for some time.
			Concurrency::CurrentThread::Yield();
		}
	}

	//Stop playing.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Stop());

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}
#endif