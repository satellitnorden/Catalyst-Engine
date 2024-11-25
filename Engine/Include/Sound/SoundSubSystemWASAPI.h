#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

#define USE_RT_AUDIO (1)

#if USE_RT_AUDIO
//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundCore.h>
#include <Sound/SoundSubSystem.h>

//Forward declarations.
class RtAudio;

class SoundSubSystemWASAPI final : public SoundSubSystem
{

public:

	/*
	*	Default constructor.
	*/
	SoundSubSystemWASAPI() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~SoundSubSystemWASAPI() NOEXCEPT
	{

	}

	/*
	*	Queries for available audio devices.
	*/
	void QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT override;

	/*
	*	Initializes this sound sub system.
	*/
	void Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT override;

	/*
	*	Terminates this sound sub system.
	*/
	void Terminate() NOEXCEPT override;

	/*
	*	Returns if this sound sub system is initialized.
	*/
	FORCE_INLINE NO_DISCARD bool IsInitialized() const NOEXCEPT override
	{
		return _Initialized.Load();
	}

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized.Load())
		{
			return 0;
		}

		ASSERT(_NumberOfChannels != 0, "Oh no!");

		return _NumberOfChannels;
	}

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSampleRate() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized.Load())
		{
			return 0.0f;
		}

		ASSERT(_SampleRate != 0.0f, "Oh no!");

		return _SampleRate;
	}

	/*
	*	Returns the sound format.
	*/
	FORCE_INLINE NO_DISCARD SoundFormat GetSoundFormat() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized.Load())
		{
			return SoundFormat::UNKNOWN;
		}

		ASSERT(_SoundFormat != SoundFormat::UNKNOWN, "Oh no!");

		return _SoundFormat;
	}

	/*
	*	Returns the buffer size.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetBufferSize() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized.Load())
		{
			return 0;
		}

		ASSERT(_BufferSize != 0, "Oh no!");

		return _BufferSize;
	}

	/*
	*	Returns the audio latency.
	*	That is, the time between a sound is requested to be played until it is heard.
	*	This gives an estimate, and might be a bit unreliable on certain platforms.
	*	The returned value is in milliseconds.
	*/
	NO_DISCARD float32 GetAudioLatency() const NOEXCEPT override;

	/*
	*	Opens an input stream on the given audio device, with the given parameters.
	*/
	void OpenInputStream
	(
		const uint32 start_channel_index,
		const uint32 number_of_channels,
		InputStreamCallback input_stream_callback,
		void *const RESTRICT user_data
	) NOEXCEPT override;

private:

	//Denotes if this sound sub system is initialized.
	Atomic<bool> _Initialized{ false };

	//The number of channels.
	uint8 _NumberOfChannels;

	//The sample rate.
	float32 _SampleRate;

	//The sound format.
	SoundFormat _SoundFormat{ SoundFormat::UNKNOWN };

	//The buffer size.
	uint32 _BufferSize;

	//The Rt Audio object.
	RtAudio *RESTRICT _RtAudio{ nullptr };

	//The opened audio device.
	AudioDevice _OpenedAudioDevice;

	/*
	*	The update function.
	*/
	void Update() NOEXCEPT;

};
#else
//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundCore.h>
#include <Sound/SoundSubSystem.h>

class SoundSubSystemWASAPI final : public SoundSubSystem
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE SoundSubSystemWASAPI() NOEXCEPT
	{
		_Type = SoundSubSystemType::WASAPI;
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~SoundSubSystemWASAPI() NOEXCEPT
	{

	}

	/*
	*	Queries for available audio devices.
	*/
	void QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT override;

	/*
	*	Initializes this sound sub system.
	*/
	void Initialize(const InitializationParameters& initialization_parameters) NOEXCEPT override;

	/*
	*	Terminates this sound sub system.
	*/
	void Terminate() NOEXCEPT override;

	/*
	*	Returns if this sound sub system is initialized.
	*/
	FORCE_INLINE NO_DISCARD bool IsInitialized() const NOEXCEPT override
	{
		return _Initialized;
	}

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized)
		{
			return 0;
		}

		ASSERT(_NumberOfChannels != 0, "Oh no!");

		return _NumberOfChannels;
	}

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSampleRate() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized)
		{
			return 0.0f;
		}

		ASSERT(_SampleRate != 0.0f, "Oh no!");

		return _SampleRate;
	}

	/*
	*	Returns the sound format.
	*/
	FORCE_INLINE NO_DISCARD SoundFormat GetSoundFormat() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized)
		{
			return SoundFormat::UNKNOWN;
		}

		ASSERT(_SoundFormat != SoundFormat::UNKNOWN, "Oh no!");

		return _SoundFormat;
	}

	/*
	*	Returns the buffer size.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetBufferSize() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized)
		{
			return 0;
		}

		ASSERT(_BufferSize != 0, "Oh no!");

		return _BufferSize;
	}

	/*
	*	Returns the audio latency.
	*	That is, the time between a sound is requested to be played until it is heard.
	*	This gives an estimate, and might be a bit unreliable on certain platforms.
	*	The returned value is in milliseconds.
	*/
	NO_DISCARD float32 GetAudioLatency() const NOEXCEPT override;

private:

	//The thread.
	Thread _Thread;

	//Denotes if this sub system should terminate.
	bool _Terminate{ false };

	//The audio endpoint.
	IMMDevice *RESTRICT _AudioEndpoint{ nullptr };

	//The audio client.
	IAudioClient *RESTRICT _AudioClient{ nullptr };

	//The audio render client.
	IAudioRenderClient *RESTRICT _AudioRenderClient{ nullptr };

	//Denotes if this sound sub system is initialized.
	Atomic<bool> _Initialized{ false };

	//The number of channels.
	uint8 _NumberOfChannels;

	//The sample rate.
	float32 _SampleRate;

	//The sound format.
	SoundFormat _SoundFormat{ SoundFormat::UNKNOWN };

	//The buffer size.
	uint32 _BufferSize;

	/*
	*	The update function.
	*/
	void Update() NOEXCEPT;

};
#endif
#endif