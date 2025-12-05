#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Backends/AudioBackend.h>
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/AudioStreamPlayer.h>
#include <Audio/AudioTrack.h>
#include <Audio/PlayAudio2DRequest.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Thread.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AudioAsset.h>

//Systems.
#include <Systems/System.h>

class AudioSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		AudioSystem,
		SYSTEM_INITIALIZE()
		SYSTEM_TERMINATE()
		SYSTEM_UPDATE(RANGE(POST, RUN_ON_MAIN_THREAD))
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AudioSystem() NOEXCEPT
	{

	}

	/*
	*	Returns the backend.
	*/
	FORCE_INLINE NO_DISCARD Audio::Backend GetBackend() const NOEXCEPT
	{
		return _RequestedBackend;
	}

	/*
	*	Sets the backend.
	*/
	FORCE_INLINE void SetBackend(const Audio::Backend value) NOEXCEPT
	{
		_RequestedBackend = value;
		InitializeBackend(_RequestedBackend);
	}

	/*
	*	Returns the default audio device.
	*/
	NO_DISCARD AudioDeviceInformation GetDefaultAudioDevice() const NOEXCEPT;

	/*
	*	Queries audio devices.
	*/
	FORCE_INLINE void QueryAudioDevices(DynamicArray<AudioDeviceInformation> *const RESTRICT audio_devices) const NOEXCEPT
	{
		_Backend->QueryAudioDevices(audio_devices);
	}

	/*
	*	Sets the audio device.
	*/
	FORCE_INLINE void SetAudioDevice(const AudioDeviceInformation &audio_device) NOEXCEPT
	{
		_RequestedAudioDevice = audio_device;
		InitializeBackend(_RequestedBackend);
	}

	/*
	*	Sets the audio device.
	*/
	FORCE_INLINE void SetAudioDevice(const DynamicString &audio_device_name) NOEXCEPT
	{
		DynamicArray<AudioDeviceInformation> audio_devices;
		QueryAudioDevices(&audio_devices);

		for (const AudioDeviceInformation &audio_device : audio_devices)
		{
			if (audio_device._Name == audio_device_name)
			{
				SetAudioDevice(audio_device);

				return;
			}
		}
	}

	/*
	*	Sets the beat per minute.
	*/
	FORCE_INLINE void SetBeatsPerMinute(const float32 value) NOEXCEPT
	{
		_BeatsPerMinute.Store(value);
	}

	/*
	*	Creates an audio effect.
	*/
	template <typename TYPE, class... Arguments>
	FORCE_INLINE NO_DISCARD TYPE *const RESTRICT CreateAudioEffect(Arguments&&... arguments) NOEXCEPT
	{
		TYPE *const RESTRICT new_effect{ new TYPE(std::forward<Arguments>(arguments)...) };
		_AllEffects.Emplace(new_effect);

		if (new_effect->WantsMainThreadUpdate())
		{
			_MainThreadUpdateEffects.Emplace(new_effect);
		}

		return new_effect;
	}

	/*
	*	Destroys the given audio effect.
	*/
	FORCE_INLINE void DestroyAudioEffect(AudioEffect *const RESTRICT effect) NOEXCEPT
	{
		if (effect->WantsMainThreadUpdate())
		{
			_MainThreadUpdateEffects.Erase<false>(effect);
		}

		_AllEffects.Erase<false>(effect);
		delete effect;
	}

	/*
	*	Adds a master audio effect.
	*/
	void AddMasterAudioEffect(AudioEffect *const RESTRICT effect) NOEXCEPT;

	/*
	*	Adds an audio track with the given information. Returns it's identifier.
	*/
	Audio::Identifier AddAudioTrack(const AudioTrackInformation &information) NOEXCEPT;

	/*
	*	Removes the audio track with the given identifier.
	*/
	void RemoveAudioTrack(const Audio::Identifier identifier) NOEXCEPT;

	/*
	*	Adds an effect to the track with the given identifier.
	*/
	void AddEffectToAudioTrack(const Audio::Identifier identifier, AudioEffect *const RESTRICT effect) NOEXCEPT;

	/*
	*	Plays the given audio (in 2D). Returns an identifier for the played audio.
	*/
	Audio::Identifier PlayAudio2D(const PlayAudio2DRequest &request) NOEXCEPT;

	/*
	*	Stops the audio 2D with the given identifier.
	*/
	void StopAudio2D(const Audio::Identifier identifier) NOEXCEPT;

	/*
	*	Retrieves the audio time for the given identifier.
	*	Please note that this is an asynchronous call - It will send a request to the audio thread and set 'ready' when it has written the value.
	*/
	void GetAudioTime(const Audio::Identifier identifier, float64 *const RESTRICT value, AtomicFlag *const RESTRICT ready) NOEXCEPT;

private:

	//Constants.
	constexpr static uint8 NUMBER_OF_MIX_BUFFERS{ 1 };

	/*
	*	Mix buffer class definition.
	*/
	class MixBuffer final
	{

	public:

		//The inputs.
		DynamicArray<DynamicArray<float32>> _Inputs;

		//The outputs.
		DynamicArray<DynamicArray<float32>> _Outputs;

		//Denotes whether or not this buffer is mixed.
		AtomicFlag _IsMixed;

	};

	/*
	*	Request class definition.
	*/
	class Request final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			NONE,

			ADD_AUDIO_TRACK,
			REMOVE_AUDIO_TRACK,
			ADD_AUDIO_EFFECT_TO_TRACK,
			PLAY_AUDIO_2D,
			STOP_AUDIO_2D,
			MIX_BUFFER,

			GET_AUDIO_TIME
		};

		//The type.
		Type _Type;

		//The add audio track data.
		struct
		{
			//The information.
			AudioTrackInformation _Information;

			//The identifier.
			Audio::Identifier _Identifier;
		} _AddAudioTrackData;

		//The remove audio track data.
		struct
		{
			//The identifier.
			Audio::Identifier _Identifier;
		} _RemoveAudioTrackData;

		//The add audio effect to track data.
		struct
		{
			//The identifier.
			Audio::Identifier _Identifier;

			//The effect.
			AudioEffect *RESTRICT _Effect;
		} _AddAudioEffectToTrackData;

		//The play audio 2D data.
		struct
		{
			//The identifier.
			Audio::Identifier _Identifier;

			//The request.
			PlayAudio2DRequest _Request;
		} _PlayAudio2DData;

		//The stop audio 2D data.
		struct
		{
			//The identifier.
			Audio::Identifier _Identifier;
		} _StopAudio2DData;

		//The mix buffer data.
		struct
		{
			//The buffer index.
			uint8 _BufferIndex;
		} _MixBufferData;

		//The get audio time data.
		struct
		{
			//The identifier.
			Audio::Identifier _Identifier;

			//The value.
			float64 *RESTRICT _Value;

			//The ready flag.
			AtomicFlag *RESTRICT _Ready;
		} _GetAudioTimeData;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Request() NOEXCEPT
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~Request() NOEXCEPT
		{

		}

	};

	/*
	*	Playing audio 2D class definition.
	*/
	class PlayingAudio2D final
	{

	public:

		//The identifier.
		Audio::Identifier _Identifier;

		//The player.
		AudioStreamPlayer _Player;

	};

	//The requested backend.
	Audio::Backend _RequestedBackend{ Audio::Backend::WASAPI };

	//The requested audio device.
	AudioDeviceInformation _RequestedAudioDevice;

	//The backend.
	AudioBackend *RESTRICT _Backend{ nullptr };

	//The sample rate.
	Atomic<float32> _SampleRate{ 48'000.0f };

	//The beats per minute.
	Atomic<float32> _BeatsPerMinute{ 120.0f };

	//The mix buffers.
	StaticArray<MixBuffer, NUMBER_OF_MIX_BUFFERS> _MixBuffers;

	//The current mix buffer index.
	uint8 _CurrentMixBufferIndex{ 0 };

	//Denotes whether or not the mix thread should mix.
	AtomicFlag _ShouldMix;

	//The mix thread.
	Thread _MixThread;

	//The requests.
	AtomicQueue<Request, 1'024, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> _Requests;

	//The audio track identifier generator.
	Audio::Identifier _AudioTrackIdentifierGenerator{ 0 };

	//The identifier generator.
	Audio::Identifier _IdentifierGenerator{ 0 };

	//The main thread audio tracks.
	DynamicArray<AudioTrack> _MainThreadAudioTracks;

	//The mix thread audio tracks.
	DynamicArray<AudioTrack> _MixThreadAudioTracks;

	//The playing audio 2D.
	DynamicArray<PlayingAudio2D> _PlayingAudio2D;

	//Container for all effects.
	DynamicArray<AudioEffect *RESTRICT> _AllEffects;

	//Container for all effects that want main thread updates.
	DynamicArray<AudioEffect *RESTRICT> _MainThreadUpdateEffects;

	/*
	*	Initializes the backend.
	*/
	void InitializeBackend(const Audio::Backend backend) NOEXCEPT;

	/*
	*	Creates the backend parameters.
	*/
	NO_DISCARD AudioBackend::Parameters CreateBackendParameters() NOEXCEPT;

	/*
	*	Terminates the backend.
	*/
	void TerminateBackend() NOEXCEPT;

	/*
	*	Mixes.
	*/
	void Mix() NOEXCEPT;

	/*
	*	Processes requests. Returns if any requests were processed.
	*/
	NO_DISCARD bool ProcessRequests() NOEXCEPT;

	/*
	*	Processes an add audio track request.
	*/
	void ProcessAddAudioTrackRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a remove audio track request.
	*/
	void ProcessRemoveAudioTrackRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes an add audio effect to track request.
	*/
	void ProcessAddAudioEffectToTrackRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a play audio 2D request.
	*/
	void ProcessPlayAudio2DRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a stop audio 2D request.
	*/
	void ProcessStopAudio2DRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a mix buffer request.
	*/
	void ProcessMixBufferRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a get audio time request.
	*/
	void ProcessGetAudioTimeRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes.
	*/
	void Process(const DynamicArray<DynamicArray<float32>> &inputs, DynamicArray<DynamicArray<float32>> *const RESTRICT outputs) NOEXCEPT;

};