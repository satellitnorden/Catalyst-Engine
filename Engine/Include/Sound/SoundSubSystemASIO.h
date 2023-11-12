#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>

//Sound.
#include <Sound/SoundCore.h>
#include <Sound/SoundSubSystem.h>

//Forward declarations.
class RtAudio;

class SoundSubSystemASIO final : public SoundSubSystem
{

public:

	/*
	*	Default constructor.
	*/
	SoundSubSystemASIO() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~SoundSubSystemASIO() NOEXCEPT
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
		return _Initialized;
	}

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized || !_OpenedStreamInformation._IsOpen)
		{
			return 0;
		}

		ASSERT(_OpenedStreamInformation._NumberOfOutputChannels != 0, "Oh no!");

		return _OpenedStreamInformation._NumberOfOutputChannels;
	}

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSampleRate() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized || !_OpenedStreamInformation._IsOpen)
		{
			return 0.0f;
		}

		ASSERT(_OpenedStreamInformation._SampleRate != 0.0f, "Oh no!");

		return _OpenedStreamInformation._SampleRate;
	}

	/*
	*	Returns the sound format.
	*/
	FORCE_INLINE NO_DISCARD SoundFormat GetSoundFormat() const NOEXCEPT override
	{
		//Don't do anything if this sound sub system isn't initialized.
		if (!_Initialized || !_OpenedStreamInformation._IsOpen)
		{
			return SoundFormat::UNKNOWN;
		}

		ASSERT(_OpenedStreamInformation._SoundFormat != SoundFormat::UNKNOWN, "Oh no!");

		return _OpenedStreamInformation._SoundFormat;
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

	/*
	*	Opened stream information class definition.
	*/
	class OpenedStreamInformation final
	{

	public:

		//Denotes whether or not this stream is open.
		bool _IsOpen{ false };

		//Denotes if this stream needs a restarts.
		AtomicFlag _NeedsRestart;

		//The first output channel.
		uint32 _FirstOutputChannel{ 0 };

		//The number of output channels.
		uint32 _NumberOfOutputChannels{ 0 };

		//The first input channel.
		uint32 _FirstInputChannel{ 0 };

		//The number of input channels.
		uint32 _NumberOfInputChannels{ 0 };

		//The sample rate.
		float32 _SampleRate{ 0.0f };

		//The sound format.
		SoundFormat _SoundFormat{ SoundFormat::UNKNOWN };

	};

	//Denotes if this sound sub system is initialized.
	Atomic<bool> _Initialized{ false };

	//The Rt Audio object.
	RtAudio* RESTRICT _RtAudio{ nullptr };

	//The opened audio device.
	AudioDevice _OpenedAudioDevice;

	//The opened stream information.
	OpenedStreamInformation _OpenedStreamInformation;

	//The update identifier.
	uint64 _UpdateIdentifier;

	//The pending opened input streams.
	AtomicQueue<class OpenedInputStream *RESTRICT, 64, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _PendingOpenedInputStreams;

	//Container for all opened input streams.
	DynamicArray<class OpenedInputStream *RESTRICT> _OpenedInputStreams;

	//The buffer size.
	uint32 _BufferSize;

	/*
	*	Sequentially updates this sound sub system.
	*/
	void SequentialUpdate() NOEXCEPT;

	/*
	*	Starts the stream.
	*/
	void StartStream() NOEXCEPT;

	/*
	*	Stops the stream.
	*/
	void StopStream() NOEXCEPT;

	/*
	*	The audio callback.
	*/
	void AudioCallback
	(
		void *const RESTRICT output_buffer,
		void *const RESTRICT input_buffer,
		const uint32 number_of_samples
	) NOEXCEPT;

};
#endif