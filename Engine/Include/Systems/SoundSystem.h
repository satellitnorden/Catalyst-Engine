#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/AudioDevice.h>
#include <Sound/InputMIDIDevice.h>
#include <Sound/MIDIMessage.h>
#include <Sound/OutputMIDIDevice.h>
#include <Sound/PlaySound2DRequest.h>
#include <Sound/PlaySound3DRequest.h>
#include <Sound/SoundCore.h>
#include <Sound/SoundMixComponent.h>
#include <Sound/SoundSubSystem.h>

//Systems.
#include <Systems/System.h>

class SoundSystem final
{

public:

	//The default number of mixing buffers.
	static constexpr uint8 DEFAULT_NUMBER_OF_MIXING_BUFFERS{ 8 };

	//The default number of samples in each mixing buffer.
	static constexpr uint32 DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER{ 128 };

	//System declaration.
	CATALYST_SYSTEM
	(
		SoundSystem
	);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT
	{
	
	}

	/*
	*	Returns the audio latency.
	*	That is, the time between a sound is requested to be played until it is heard.
	*	This gives an estimate, and might be a bit unreliable on certain platforms.
	*	The returned value is in milliseconds.
	*/
	uint32 GetAudioLatency() const NOEXCEPT;

	/*
	*	Queries for input MIDI devices.
	*/
	void QueryMIDIDevices(DynamicArray<InputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT;

	/*
	*	Queries for output MIDI devices.
	*/
	void QueryMIDIDevices(DynamicArray<OutputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT;

	/*
	*	Opens an input MIDI device.
	*/
	void OpenMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT;

	/*
	*	Opens an output MIDI device.
	*/
	void OpenMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT;

	/*
	*	Retrieves a MIDI message from the queue of the specified input MIDI device. Returns whether or not a message was retrieved from the queue.
	*/
	NO_DISCARD bool RetrieveMIDIMessage(InputMIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT;

	/*
	*	Sends a MIDI message to the specified output MIDI device.
	*/
	void SendMIDIMessage(OutputMIDIDevice *const RESTRICT midi_device, const MIDIMessage &midi_message) NOEXCEPT;

	/*
	*	Closes an input MIDI device.
	*/
	void CloseMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT;

	/*
	*	Closes an output MIDI device.
	*/
	void CloseMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT;

private:

	//The configuration.
	CatalystProjectSoundConfiguration _Configuration;

	//The sound instance counter.
	uint64 _SoundInstanceCounter{ 1 };

	//Container for the master channel mix components.
	StaticArray<DynamicArray<SoundMixComponent>, 2> _MasterChannelMixComponents;

	//The mixing thread.
	Thread _MixingThread;

	//Denotes whether or not the mixing buffers are initialized.
	bool _MixingBuffersInitialized{ false };

	//The number of mixing buffers.
	uint8 _NumberOfMixingBuffers{ 0 };

	//The number of samples per mixing buffer.
	uint32 _NumberOfSamplesPerMixingBuffer{ 0 };

	//The mixing buffers.
	DynamicArray<void *RESTRICT> _MixingBuffers;

	//The intermediate mixing buffer.
	float32 *RESTRICT _IntermediateMixingBuffer;

	//The current mixing buffer write index.
	uint8 _CurrentMixingBufferWriteIndex{ 0 };

	//The current mixing buffer read index.
	uint8 _CurrentMixingBufferReadIndex{ 0 };

	//The amount of mixing buffers ready.
	Atomic<int32> _MixingBuffersReady{ 0 };

	//The current sample read index.
	uint32 _CurrentSampleReadIndex{ 0 };

	//Denotes whether or not the sound system should currently mix.
	AtomicFlag _ShouldMix;

	//Denotes whether or not the sound system is currently mixing.
	AtomicFlag _IsMixing;

	//Denotes whetehr or not the sound system is currently muted.
	AtomicFlag _IsMuted;

	//Denotes whetehr or not the sound system is currently paused.
	AtomicFlag _IsPaused;

	//Denotes whether or not the sound system should currently record.
	AtomicFlag _ShouldRecord;

	//Denotes whether or not the sound system is currently recording.
	AtomicFlag _IsRecording;

	//The recording sound asset.
	SoundAsset *RESTRICT _RecordingSoundAsset;

	//Denotes whether or not the sound system should continue while the engine is paused.
	bool _ContinueWhileEnginePaused{ false };

	//The sub system.
	SoundSubSystem *RESTRICT _SubSystem;

	/*
	*	Initializes the platform.
	*/
	void PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT;

	/*
	*	Terminates the platform.
	*/
	void PlatformTerminate() NOEXCEPT;

	//Friend declarations.
#if defined(CATALYST_PLATFORM_WINDOWS)
	friend class SoundSubSystemWASAPI;
	friend class SoundSubSystemASIO;
#endif

};