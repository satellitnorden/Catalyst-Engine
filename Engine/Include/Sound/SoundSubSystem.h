#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Sound.
#include <Sound/SoundCore.h>
#include <Sound/AudioDevice.h>

//Forward declarations.
class SoundSystem;

class SoundSubSystem
{

public:

	/*
	*	Initialization parameters class definition.
	*/
	class InitializationParameters final
	{

	public:

		//The audio device. If nullptr, the sub system will pick the default one. Otherwise, this one will be used.
		AudioDevice *RESTRICT _AudioDevice;

	};

	//The type.
	SoundSubSystemType _Type;

	//The sound system.
	SoundSystem *RESTRICT _SoundSystem;

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~SoundSubSystem() NOEXCEPT
	{

	}

	/*
	*	Queries for available audio devices.
	*/
	virtual void QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT = 0;

	/*
	*	Initializes this sound sub system.
	*/
	virtual void Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT = 0;

	/*
	*	Terminates this sound sub system.
	*/
	virtual void Terminate() NOEXCEPT = 0;

	/*
	*	Returns if this sound sub system is initialized.
	*/
	virtual NO_DISCARD bool IsInitialized() const NOEXCEPT = 0;

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	virtual NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT = 0;

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	virtual NO_DISCARD float32 GetSampleRate() const NOEXCEPT = 0;

	/*
	*	Returns the sound format.
	*/
	virtual NO_DISCARD SoundFormat GetSoundFormat() const NOEXCEPT = 0;

	/*
	*	Returns the buffer size.
	*/
	virtual NO_DISCARD uint32 GetBufferSize() const NOEXCEPT = 0;

	/*
	*	Returns the audio latency.
	*	That is, the time between a sound is requested to be played until it is heard.
	*	This gives an estimate, and might be a bit unreliable on certain platforms.
	*	The returned value is in milliseconds.
	*/
	virtual NO_DISCARD float32 GetAudioLatency() const NOEXCEPT = 0;

};