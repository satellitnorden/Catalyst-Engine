#if defined(USE_NEW_AUDIO_SYSTEM)
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
		SYSTEM_DEFINED_REQUIREMENT(USE_NEW_AUDIO_SYSTEM)
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
	*	Creates an audio effect.
	*/
	template <typename TYPE, class... Arguments>
	FORCE_INLINE NO_DISCARD TYPE *const RESTRICT CreateAudioEffect(Arguments&&... arguments) NOEXCEPT
	{
		TYPE *const RESTRICT new_effect{ new TYPE(std::forward<Arguments>(arguments)...) };
		_AllEffects.Emplace(new_effect);
		return new_effect;
	}

	/*
	*	Destroys the given audio effect.
	*/
	FORCE_INLINE void DestroyAudioEffect(AudioEffect *const RESTRICT effect) NOEXCEPT
	{
		_AllEffects.Erase<false>(effect);
		delete effect;
	}

	/*
	*	Adds a master audio effect.
	*/
	void AddMasterAudioEffect(AudioEffect *const RESTRICT effect) NOEXCEPT;

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
			
			ADD_AUDIO_EFFECT_TO_TRACK,
			PLAY_AUDIO_2D,
			STOP_AUDIO_2D,
			MIX_BUFFER,

			GET_AUDIO_TIME
		};

		//The type.
		Type _Type;

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

	//The backend.
	AudioBackend *RESTRICT _Backend{ nullptr };

	//The mix buffers.
	StaticArray<MixBuffer, NUMBER_OF_MIX_BUFFERS> _MixBuffers;

	//The current mix buffer index.
	uint8 _CurrentMixBufferIndex{ 0 };

	//The mix thread.
	Thread _MixThread;

	//The requests.
	AtomicQueue<Request, 1'024, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> _Requests;

	//The audio track identifier generator.
	Audio::Identifier _AudioTrackIdentifierGenerator{ 0 };

	//The identifier generator.
	Audio::Identifier _IdentifierGenerator{ 0 };

	//The audio tracks.
	DynamicArray<AudioTrack> _AudioTracks;

	//The playing audio 2D.
	DynamicArray<PlayingAudio2D> _PlayingAudio2D;

	//Container for all effects.
	DynamicArray<AudioEffect *RESTRICT> _AllEffects;

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
#endif