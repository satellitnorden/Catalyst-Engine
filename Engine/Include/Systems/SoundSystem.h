#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/MIDIDevice.h>
#include <Sound/PlaySoundRequest.h>
#include <Sound/SoundCore.h>
#include <Sound/SoundMixComponent.h>

class SoundSystem final
{

public:

	//The number of mixing buffers.
	static constexpr uint8 NUMBER_OF_MIXING_BUFFERS{ 8 };

	//The number of samples in each mixing buffer.
	static constexpr uint32 NUMBER_OF_SAMPLES_PER_MIXING_BUFFER{ 256 };

	//Declare the singleton.
	DECLARE_SINGLETON(SoundSystem);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT
	{
	
	}

	/*
	*	Initializes the sound system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the sound system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	uint8 GetNumberOfChannels() const NOEXCEPT;

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	float32 GetSampleRate() const NOEXCEPT;

	/*
	*	Returns the number of bits per sample.
	*/
	uint8 GetNumberOfBitsPerSample() const NOEXCEPT;

	/*
	*	Adds a mix component to the master mix channel. Returns the unique identifier for that sound mix component.
	*/
	uint64 AddMasterChannelSoundMixComponent(const SoundMixComponent &component) NOEXCEPT;

	/*
	*	Removes a sound mix component from the master mix channel.
	*/
	void RemoveMasterChannelSoundMixComponent(const uint64 identifier) NOEXCEPT;

	/*
	*	Plays a sound.
	*/
	void PlaySound(const PlaySoundRequest &request) NOEXCEPT;

	/*
	*	Stops a sound.
	*/
	void StopSound(const SoundInstanceHandle handle) NOEXCEPT;

	/*
	*	Returns whether or not the sound system is currently muted.
	*/
	NO_DISCARD bool IsCurrentlyMuted() const NOEXCEPT;

	/*
	*	Mutes the sound system.
	*/
	void Mute() NOEXCEPT;

	/*
	*	Un-mutes the sound system.
	*/
	void UnMute() NOEXCEPT;

	/*
	*	Returns whether or not the sound system is currently paused.
	*/
	NO_DISCARD bool IsCurrentlyPaused() const NOEXCEPT;

	/*
	*	Pauses the sound system.
	*/
	void Pause() NOEXCEPT;

	/*
	*	Un-pauses the sound system.
	*/
	void UnPause() NOEXCEPT;

	/*
	*	Returns if the sound system is currently recording.
	*/
	NO_DISCARD bool IsCurrentlyRecording() const NOEXCEPT;

	/*
	*	Starts recording.
	*	Can report the expected length, in seconds, to give the sound system a heads up about how much memory needs to be allocated.
	*/
	void StartRecording(const float32 expected_length = 0.0f) NOEXCEPT;

	/*
	*	Stops recording. Saves the recording to a .WAV file to the specified file path.
	*/
	void StopRecording(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Queries for MIDI devices.
	*/
	void QueryMIDIDevices(DynamicArray<MIDIDevice> *const RESTRICT midi_devices) NOEXCEPT;

private:

	//The sound instance counter.
	uint64 _SoundInstanceCounter{ 1 };

	//Container for the master channel mix components.
	DynamicArray<SoundMixComponent> _MasterChannelMixComponents;

	//The mixing thread.
	Thread _MixingThread;

	//Denotes whether or not the mixing buffers are initialized.
	bool _MixingBuffersInitialized{ false };

	//The mixing buffers.
	StaticArray<void *RESTRICT, NUMBER_OF_MIXING_BUFFERS> _MixingBuffers;

	//The current mixing buffer write index.
	uint8 _CurrentMixingBufferWriteIndex{ 0 };

	//The current mixing buffer read index.
	uint8 _CurrentMixingBufferReadIndex{ 0 };

	//The amount of mixing buffers ready.
	Atomic<int32> _MixingBuffersReady{ 0 };

	//The current sample read index.
	uint32 _CurrentSampleReadIndex{ 0 };

	//Denotes whetehr or not the sound system is currently muted.
	AtomicFlag _IsMuted;

	//Denotes whetehr or not the sound system is currently paused.
	AtomicFlag _IsPaused;

	//Denotes whether or not the sound system should currently record.
	AtomicFlag _ShouldRecord;

	//Denotes whether or not the sound system is currently recording.
	AtomicFlag _IsRecording;

	//The recording sound resource.
	SoundResource _RecordingSoundResource;

	/*
	*	Initializes the platform.
	*/
	void PlatformInitialize() NOEXCEPT;

	/*
	*	Returns if the platform is initialized.
	*/
	NO_DISCARD bool PlatformInitialized() const NOEXCEPT;

	/*
	*	Terminates the platform.
	*/
	void PlatformTerminate() NOEXCEPT;

	/*
	*	Performs mixing.
	*/
	void Mix() NOEXCEPT;

	/*
	*	The asynchronous update function.
	*/
	void AsynchronousUpdate() NOEXCEPT;

	/*
	*	The sound callback.
	*/
	void SoundCallback(	const float32 sample_rate,
						const uint8 bits_per_sample,
						const uint8 number_of_channels,
						const uint32 number_of_samples,
						void *const RESTRICT buffer_data) NOEXCEPT;

};