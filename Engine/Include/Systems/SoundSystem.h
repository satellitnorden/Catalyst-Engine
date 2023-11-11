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
#include <Sound/PlaySoundRequest.h>
#include <Sound/SoundCore.h>
#include <Sound/SoundMixComponent.h>
#include <Sound/SoundSubSystem.h>

class ALIGN(8) SoundSystem final
{

public:

	//The default number of mixing buffers.
	static constexpr uint8 DEFAULT_NUMBER_OF_MIXING_BUFFERS{ 8 };

	//The default number of samples in each mixing buffer.
	static constexpr uint32 DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER{ 128 };

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
	void Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT;

	/*
	*	Terminates the sound system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Sets the sound sub system type.
	*/
	void SetSoundSubSystemType(const SoundSubSystemType sound_sub_system_type) NOEXCEPT;

	/*
	*	Queries for audio devices.
	*/
	void QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT;

	/*
	*	Opens the given audio device.
	*/
	void OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT;

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	uint8 GetNumberOfChannels() const NOEXCEPT;

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	float32 GetSampleRate() const NOEXCEPT;

	/*
	*	Returns the sound format.
	*/
	SoundFormat GetSoundFormat() const NOEXCEPT;

	/*
	*	Returns the audio latency.
	*	That is, the time between a sound is requested to be played until it is heard.
	*	This gives an estimate, and might be a bit unreliable on certain platforms.
	*	The returned value is in milliseconds.
	*/
	uint32 GetAudioLatency() const NOEXCEPT;

	/*
	*	Opens an input stream on the given audio device, with the given parameters.
	*/
	void OpenInputStream
	(
		AudioDevice *const RESTRICT audio_device,
		const uint32 start_channel_index,
		const uint32 number_of_channels,
		InputStreamCallback input_stream_callback,
		void *const RESTRICT user_data
	) NOEXCEPT;

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
	void StartRecording(SoundResource *const RESTRICT recording_sound_resource, const float32 expected_length = 0.0f) NOEXCEPT;

	/*
	*	Stops recording. Saves the recording to a .WAV file to the specified file path.
	*/
	void StopRecording() NOEXCEPT;

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

	/*
	*	Returns whether or not the sound system should continue while the engine is paused.
	*/
	FORCE_INLINE NO_DISCARD bool GetContinueWhileEnginePaused() const NOEXCEPT
	{
		return _ContinueWhileEnginePaused;
	}

	/*
	*	Returns whether or not the sound system should continue while the engine is paused.
	*/
	FORCE_INLINE void SetContinueWhileEnginePaused(const bool value) NOEXCEPT
	{
		_ContinueWhileEnginePaused = value;
	}

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

	//The recording sound resource.
	SoundResource *RESTRICT _RecordingSoundResource;

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

	/*
	*	Initializes the mixing buffers.
	*/
	void InitializeMixingBuffers(const uint8 number_of_mixing_buffers, const uint32 number_of_samples_per_mixing_buffer) NOEXCEPT;

	/*
	*	Terminates the mixing buffers.
	*/
	void TerminateMixingBuffers() NOEXCEPT;

	/*
	*	Performs mixing.
	*/
	void Mix() NOEXCEPT;

	/*
	*	The sound callback.
	*/
	void SoundCallback(	const float32 sample_rate,
						const SoundFormat sound_format,
						const uint8 number_of_channels,
						const uint32 number_of_samples,
						void *const RESTRICT buffer_data) NOEXCEPT;

	//Friend declarations.
#if defined(CATALYST_PLATFORM_WINDOWS)
	friend class SoundSubSystemWASAPI;
	friend class SoundSubSystemASIO;
#endif

};