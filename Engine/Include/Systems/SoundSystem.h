#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Data/SoundBankData.h>

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
	void Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT;

	/*
	*	Updates the sound system during the post update phase.
	*/
	void PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

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
	*	Returns the playback state of a sound instance.
	*/
	PlaybackState GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Sets the position of a sound instance.
	*/
	void SetPosition(const SoundInstanceHandle instance, const Vector3<float> &position) NOEXCEPT;

};