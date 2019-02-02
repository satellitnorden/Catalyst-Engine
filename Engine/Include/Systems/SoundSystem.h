#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

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
	*	Returns the playback state of a sound instance.
	*/
	PlaybackState GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT;

	/*
	*	Sets the parameter on the given sound instance at the given index.
	*	The index of a parameter might not always be the same as it appears in the sound editor.
	*/
	void SetParameterAtIndex(const SoundInstanceHandle instance, const uint32 index, const float value) NOEXCEPT;

	///////////////////////////////
	// Sound playback interface. //
	///////////////////////////////

	/*
	*	Plays a two dimensional sound.
	*	Can supply an optional handle to the sound instance to keep track of the sound properties.
	*	If not supplied, a temporary instance will be created that will be destroyed as soon as the sound has finished playing.
	*/
	void PlayTwoDimensionalSound(const SoundDescriptionHandle description, SoundInstanceHandle *const RESTRICT instance) NOEXCEPT;

};