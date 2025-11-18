#if defined(USE_NEW_AUDIO_SYSTEM)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Backends/AudioBackend.h>
#include <Audio/AudioStreamPlayer.h>

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
		SYSTEM_DEFINED_REQUIREMENT(USE_NEW_AUDIO_SYSTEM)
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AudioSystem() NOEXCEPT
	{

	}

	/*
	*	Plays the given audio (in 2D).
	*/
	void PlayAudio2D
	(
		const AssetPointer<AudioAsset> asset,
		const float32 gain = 1.0f
	) NOEXCEPT;

private:

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

			MIX_BUFFER,
			PLAY_AUDIO_2D
		};

		//The type.
		Type _Type;

		//The mix buffer data.
		struct
		{
			//The buffer index.
			uint8 _BufferIndex;
		} _MixBufferData;

		//The play audio 2D data.
		struct
		{
			//The asset.
			AssetPointer<AudioAsset> _Asset;

			//The gain.
			float32 _Gain;
		} _PlayAudio2DData;

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

	//The backend.
	AudioBackend *RESTRICT _Backend{ nullptr };

	//The mix buffers.
	StaticArray<MixBuffer, 2> _MixBuffers;

	//The current mix buffer index.
	uint8 _CurrentMixBufferIndex{ 0 };

	//The mix thread.
	Thread _MixThread;

	//The requests.
	AtomicQueue<Request, 1'024, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> _Requests;

	//The 2D players.
	DynamicArray<AudioStreamPlayer> _2DPlayers;

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
	*	Processes a mix buffer request.
	*/
	void ProcessMixBufferRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes a play audio 2D request.
	*/
	void ProcessPlayAudio2DRequest(const Request &request) NOEXCEPT;

	/*
	*	Processes.
	*/
	void Process(const DynamicArray<DynamicArray<float32>> &inputs, DynamicArray<DynamicArray<float32>> *const RESTRICT outputs) NOEXCEPT;

};
#endif