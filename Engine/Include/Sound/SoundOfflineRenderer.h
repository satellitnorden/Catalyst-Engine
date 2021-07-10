#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Sound.
#include <Sound/PlaySoundRequest.h>
#include <Sound/SoundMixComponent.h>
#include <Sound/SoundResourcePlayer.h>

class SoundOfflineRenderer final
{

public:

	//Type aliases.
	using Callback = void(*)(SoundOfflineRenderer *const RESTRICT sound_offline_renderer);

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The requested length of the sound file, in seconds.
		float64 _RequestedLength;

		//The callback.
		Callback _Callback;

		//The sound mix components.
		DynamicArray<SoundMixComponent> _SoundMixComponents;

	};

	/*
	*	Playing sound class definition.
	*/
	class PlayingSound final
	{

	public:

		//The sound resource players.
		SoundResourcePlayer _SoundResourcePlayer;

		//The sound instance handle.
		SoundInstanceHandle _SoundInstanceHandle;

		//The sound stopped callback.
		SoundStoppedCallback _SoundStoppedCallback;

	};

	/*
	*	Initializes this sound offline renderer.
	*/
	void Initialize(const Parameters &parameters) NOEXCEPT;

	/*
	*	Updates this sound offline renderer. Returns the current progress. It is done when it is >= 1.0f.
	*/
	NO_DISCARD float32 Update() NOEXCEPT;

	/*
	*	Terminates this sound offline renderer.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Plays a sound.
	*/
	void PlaySound(const PlaySoundRequest &request) NOEXCEPT;

	/*
	*	Stops a sound.
	*/
	void StopSound(const SoundInstanceHandle handle) NOEXCEPT;

private:

	//The output file path.
	DynamicString _OutputFilePath;

	//The callback.
	Callback _Callback;

	//The sound mix components.
	DynamicArray<SoundMixComponent> _SoundMixComponents;

	//The sound instance counter.
	uint64 _SoundInstanceCounter{ 0 };

	//The playing sounds.
	DynamicArray<PlayingSound> _PlayingSounds;

	//The sound resource.
	SoundResource _SoundResource;

};