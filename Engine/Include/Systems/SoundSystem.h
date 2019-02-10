#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/Vector3.h>

//Resources.
#include <Resources/SoundBankData.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class SoundSystem final
{

public:

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
	*	Updates the sound system.
	*/
	void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Terminates the sound system.
	*/
	void Terminate() NOEXCEPT;

	//////////////////////////
	// Soundbank interface. //
	//////////////////////////

	/*
	*	Creates a sound bank.
	*/
	void CreateSoundBank(const SoundBankData &data, SoundBankHandle *const RESTRICT handle) NOEXCEPT;

	//////////////////////////////////
	// Sound description interface. //
	//////////////////////////////////

	/*
	*	Given an ID, returns the sound description.
	*/
	void GetSoundDescription(const char *const RESTRICT ID, SoundDescriptionHandle *const RESTRICT handle) NOEXCEPT;

	///////////////////////////////
	// Sound instance interface. //
	///////////////////////////////

	/*
	*	Creates a sound instance based on the given sound description.
	*/
	void CreateSoundInstance(const SoundDescriptionHandle description, SoundInstanceHandle *const RESTRICT instance) NOEXCEPT;

	/*
	*	Destroys a sound instance as soon as the sound instance has stopped playing.
	*/
	void DestroySoundInstance(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Plays a sound instance.
	*/
	void Play(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Stops a sound instance.
	*/
	void Stop(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Returns the parameter index of a parameter of the given sound instance.
	*/
	uint32 GetParameterIndex(const char *const RESTRICT parameter, const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Returns the playback state of a sound instance.
	*/
	PlaybackState GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Sets the parameter on the given sound instance at the given index.
	*	The index of a parameter might not always be the same as it appears in the sound editor.
	*/
	void SetParameterAtIndex(const SoundInstanceHandle instance, const uint32 index, const float value) NOEXCEPT;

	/*
	*	Sets the position of a sound instance.
	*/
	void SetPosition(const SoundInstanceHandle instance, const Vector3<float> &position) NOEXCEPT;

};