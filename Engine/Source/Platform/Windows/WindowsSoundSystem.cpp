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
#include <Systems/MemorySystem.h>

//Third party.
#include <ThirdParty/rtmidi/RtMidi.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Windows sound system constants.
namespace WindowsSoundSystemConstants
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

//Windows sound system data.
namespace WindowsSoundSystemData
{
	//The thread.
	Thread _Thread;

	//Denotes if the Windows sound system is initialized.
	Atomic<bool> _Initialized{ false };

	//The audio endpoint.
	IMMDevice *RESTRICT _AudioEndpoint{ nullptr };

	//The audio client.
	IAudioClient *RESTRICT _AudioClient{ nullptr };

	//The audio render client.
	IAudioRenderClient *RESTRICT _AudioRenderClient{ nullptr };

	//The number of channels.
	uint8 _NumberOfChannels;

	//The sample rate.
	float32 _SampleRate;

	//The number of bits per sample.
	uint8 _NumberOfBitsPerSample;

	//The query MIDI in.
	RtMidiIn *RESTRICT _QueryMIDIIn{ nullptr };

}

//Windows sound system logic.
namespace WindowsSoundSystemLogic
{

	/*
	*	Calculates the optimal device period.
	*/
	NO_DISCARD REFERENCE_TIME CalculateOptimalDevicePeriod(const DWORD sample_rate) NOEXCEPT
	{
		return static_cast<REFERENCE_TIME>(	/* Seconds */static_cast<float32>(SoundSystem::NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) / static_cast<float32>(sample_rate)
											/* Milliseconds */ * 1'000.0f
											/* Nanoseconds */ * 1'000'000.0f
											/* REFERENCE_TIME (100 nanoseconds) */ / 100.0f);
	}

}

/*
*	Initializes the platform.
*/
void SoundSystem::PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set up the thread.
	switch (configuration._SoundSystemMode)
	{
		case SoundSystemMode::DEFAULT:
		{
			WindowsSoundSystemData::_Thread.SetFunction([]()
			{
				SoundSystem::Instance->DefaultAsynchronousUpdate();
			});

			break;
		}

		case SoundSystemMode::LOW_LATENCY:
		{
			WindowsSoundSystemData::_Thread.SetFunction([]()
			{
				SoundSystem::Instance->LowLatencyAsynchronousUpdate();
			});

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	
	WindowsSoundSystemData::_Thread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	WindowsSoundSystemData::_Thread.SetName("Sound System - Platform Thread");
#endif

	//Launch the thread!
	WindowsSoundSystemData::_Thread.Launch();
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
	//Wait for the thread to finish.
	WindowsSoundSystemData::_Thread.Join();

	//Release the audio end point.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioEndpoint);

	//Release the audio client.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioClient);

	//Release the audio render client.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioRenderClient);
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
*	Returns the number of bits per sample.
*/
uint8 SoundSystem::GetNumberOfBitsPerSample() const NOEXCEPT
{
	//Don't do anything if the Windows sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	ASSERT(WindowsSoundSystemData::_NumberOfBitsPerSample != 0, "Oh no!");

	return WindowsSoundSystemData::_NumberOfBitsPerSample;
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
}

/*
*	Retrieves a MIDI message from the queue of the specified MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(MIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message)
{
	//Get the message.
	std::vector<uint8> message;
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
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { ASSERT(false, "Windows Catalyst sound system couldn't be initialized!"); goto CLEANUP; }

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

		goto CLEANUP;
	}

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

	//Set the number of bits per sample.
	WindowsSoundSystemData::_NumberOfBitsPerSample = static_cast<uint8>(chosen_mix_format->wBitsPerSample);

	//The Windows sound system is successfully initialized!
	WindowsSoundSystemData::_Initialized = true;

	//Start playing.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Start());

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
							static_cast<uint8>(chosen_mix_format->wBitsPerSample),
							static_cast<uint8>(chosen_mix_format->nChannels),
							number_of_samples_available,
							buffer_data);

			//Release the buffer.
			HANDLE_ERROR(WindowsSoundSystemData::_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			//Sleep for some time.
			Concurrency::CurrentThread::SleepFor(1'000);
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

/*
*	The low latency asynchronous update function.
*/
void SoundSystem::LowLatencyAsynchronousUpdate() NOEXCEPT
{
	BREAKPOINT();
}
#endif