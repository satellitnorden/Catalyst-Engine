#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>
#include <Sound/SoundSubSystemASIO.h>
#include <Sound/SoundSubSystemWASAPI.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>

//Third party.
#include <ThirdParty/RtAudio/RtAudio.h>
#include <ThirdParty/RtMidi/RtMidi.h>

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

	//The buffer size.
	uint32 _BufferSize;

	//The query MIDI in.
	RtMidiIn *RESTRICT _QueryMIDIIn{ nullptr };

	//The query MIDI out.
	RtMidiOut *RESTRICT _QueryMIDIOut{ nullptr };

	//The opened MIDI ins.
	DynamicArray<RtMidiIn *RESTRICT> _OpenedMIDIIns;

	//The opened MIDI outs.
	DynamicArray<RtMidiOut *RESTRICT> _OpenedMIDIOuts;

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
	//Create the sub system.
	switch (configuration._SoundSystemMode)
	{
		case SoundSystemMode::DEFAULT:
		{
			_SubSystem = new SoundSubSystemWASAPI();

			break;
		}

		case SoundSystemMode::LOW_LATENCY:
		{
			_SubSystem = new SoundSubSystemASIO();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the sound system.
	_SubSystem->_SoundSystem = this;

	//Initialize the sub system!
	{
		SoundSubSystem::InitializationParameters initialization_parameters;

		_SubSystem->Initialize(initialization_parameters);
	}

	/*
	//Set the Rt Audio API.
	switch (configuration._SoundSystemMode)
	{
		case SoundSystemMode::DEFAULT:
		{
			WindowsSoundSystemData::_RtAudioAPI = RtAudio::Api::WINDOWS_WASAPI;

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

	//Is this the low latency sound system mode?
	if (configuration._SoundSystemMode == SoundSystemMode::LOW_LATENCY)
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
	*/
}

/*
*	Terminates the platform.
*/
void SoundSystem::PlatformTerminate() NOEXCEPT
{
	//Terminate the sub system.
	_SubSystem->Terminate();

	//Destroy the sub system.
	delete _SubSystem;

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

	//Destroy the opened MIDI outs.
	for (RtMidiOut *const RESTRICT opened_midi_out : WindowsSoundSystemData::_OpenedMIDIOuts)
	{
		opened_midi_out->closePort();
		opened_midi_out->~RtMidiOut(); //The MemorySystem will take care of actually freeing the memory. (:
	}
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
uint32 SoundSystem::GetAudioLatency() const NOEXCEPT
{
	//Can't query the audio latency if the sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	//Go over the steps and calculate the final latency.
	float32 final_latency{ 0 };

	//First off, ask the audio client for the stream latency.
	{
		REFERENCE_TIME stream_latency;

		if (WindowsSoundSystemData::_AudioClient->GetStreamLatency(&stream_latency) == S_OK)
		{
			//Hard-coded 30 ms adjustment here.
			final_latency += static_cast<float32>(stream_latency) / 10'000.0f + 30.0f;
		}
	}

	//Secondly, add the buffer size delay.
	final_latency += static_cast<float32>(WindowsSoundSystemData::_BufferSize) / GetSampleRate() * 1'000.0f;

	//Lastly, add the time it takes for the sound system to process sounds.
	final_latency += (static_cast<float32>(DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) * static_cast<float32>(DEFAULT_NUMBER_OF_MIXING_BUFFERS)) / GetSampleRate() * 1'000.0f;

	//Return the final latency.
	return CatalystBaseMath::Round<uint32>(final_latency);
}

/*
*	Queries for input MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<InputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
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
		InputMIDIDevice &midi_device{ midi_devices->Back() };

		midi_device._Handle = nullptr;
		midi_device._Index = i;

		std::string port_name{ WindowsSoundSystemData::_QueryMIDIIn->getPortName(i) };

		midi_device._Name = port_name.substr(0, port_name.find_last_of(' ')).c_str();
	}
}

/*
*	Queries for output MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<OutputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Create the query MIDI out, if necessary.
	if (!WindowsSoundSystemData::_QueryMIDIOut)
	{
		WindowsSoundSystemData::_QueryMIDIOut = new (MemorySystem::Instance->TypeAllocate<RtMidiOut>()) RtMidiOut();

		ASSERT(WindowsSoundSystemData::_QueryMIDIOut, "Couldn't create MIDI out!");
	}

	//Query the port count.
	const uint32 port_count{ WindowsSoundSystemData::_QueryMIDIOut->getPortCount() };

	//Reserve the appropriate amount.
	midi_devices->Reserve(port_count);

	//Fill in the midi devices.
	for (uint32 i{ 0 }; i < port_count; ++i)
	{
		midi_devices->Emplace();
		OutputMIDIDevice &midi_device{ midi_devices->Back() };

		midi_device._Handle = nullptr;
		midi_device._Index = i;

		std::string port_name{ WindowsSoundSystemData::_QueryMIDIOut->getPortName(i) };

		midi_device._Name = port_name.substr(0, port_name.find_last_of(' ')).c_str();
	}
}

/*
*	Opens an input MIDI device.
*/
void SoundSystem::OpenMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Create the MIDI in.
	midi_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtMidiIn>()) RtMidiIn();

	//Open the port.
	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->openPort(midi_device->_Index);

	//Add to the opened MIDI ins.
	WindowsSoundSystemData::_OpenedMIDIIns.Emplace(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));
}

/*
*	Opens an output MIDI device.
*/
void SoundSystem::OpenMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Create the MIDI out.
	midi_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtMidiOut>()) RtMidiOut();

	//Open the port.
	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->openPort(midi_device->_Index);

	//Add to the opened MIDI outs.
	WindowsSoundSystemData::_OpenedMIDIOuts.Emplace(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));
}

/*
*	Retrieves a MIDI message from the queue of the specified input MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(InputMIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT
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
*	Sends a MIDI message to the specified output MIDI device.
*/
void SoundSystem::SendMIDIMessage(OutputMIDIDevice *const RESTRICT midi_device, const MIDIMessage &midi_message) NOEXCEPT
{
	//Construct the message.
	static thread_local DynamicArray<uint8> message;
	message.Clear();

	switch (midi_message._Type)
	{
		case MIDIMessage::Type::NOTE_ON:
		{
			message.Emplace(0x90 | (0x0f & midi_message._NoteOnChannel));
			message.Emplace(midi_message._NoteOnNote & 0x7f);
			message.Emplace(midi_message._NoteOnVelocity & 0x7f);

			break;
		}

		case MIDIMessage::Type::NOTE_OFF:
		{
			message.Emplace(0x80 | (0x0f & midi_message._NoteOffChannel));
			message.Emplace(midi_message._NoteOffNote & 0x7f);
			message.Emplace(0x00);

			break;
		}

		case MIDIMessage::Type::AFTERTOUCH:
		{
			message.Emplace(0xf0);
			message.Emplace(midi_message._AftertouchNote & 0x7f);
			message.Emplace(0);

			break;
		}

		case MIDIMessage::Type::CONTROL_CHANGE:
		{
			message.Emplace(0xb0);
			message.Emplace(midi_message._ControlChangeNote & 0x7f);
			message.Emplace(midi_message._ControlChangeValue & 0x7f);

			break;
		}

		case MIDIMessage::Type::PROGRAM_CHANGE:
		{
			message.Emplace(0xc0 | (0x0f & midi_message._ProgramChangeChannel));
			message.Emplace(midi_message._ProgramChangeValue & 0x7f);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->sendMessage(message.Data(), message.Size());
}

/*
*	Closes an input MIDI device.
*/
void SoundSystem::CloseMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Close the port.
	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->closePort();

	//Destroy the MIDI in.
	MemorySystem::Instance->TypeFree<RtMidiIn>(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));

	//Remove from the opened MIDI ins.
	WindowsSoundSystemData::_OpenedMIDIIns.Erase<false>(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));
}

/*
*	Closes an output MIDI device.
*/
void SoundSystem::CloseMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Close the port.
	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->closePort();

	//Destroy the MIDI out.
	MemorySystem::Instance->TypeFree<RtMidiOut>(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));

	//Remove from the opened MIDI outs.
	WindowsSoundSystemData::_OpenedMIDIOuts.Erase<false>(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));
}
#endif