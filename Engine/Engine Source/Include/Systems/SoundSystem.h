#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Sound.
#include <Sound/SoundCore.h>
#include <Sound/SoundRequest.h>

class SoundSystem final
{

public:

	//Declare the system.
	DECLARE_SYSTEM(SoundSystem);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~SoundSystem() NOEXCEPT;

	/*
	*	Initializes the sound system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the sound system synschronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the sound system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Loads an FMOD bank into memory.
	*/
	void LoadBank(const char *const RESTRICT filePath) NOEXCEPT;

	/*
	*	Given an event name, returns the event description.
	*/
	const EventDescription *const RESTRICT GetEventDescription(const char *const RESTRICT eventName) NOEXCEPT;

	/*
	*	Submits a sound request.
	*/
	void SubmitSoundRequest(const SoundRequest &newSoundRequest) NOEXCEPT;

private:

	//The update semaphore.
	Semaphore updateSemaphore{ SemaphoreCreationFlags::Signalled };

	//The fmod studio system.
	FMOD::Studio::System *RESTRICT studioSystem;

	//Container for all banks that is loaded at runtime.
	DynamicArray<FMOD::Studio::Bank *RESTRICT> banks;

	//Defines the current synchronous sound system buffer.
	uint8 currentSynchronousSoundSystemBuffer{ 0 };

	//Defines the current asynchronous sound system buffer.
	uint8 currentAsynchronousSoundSystemBuffer{ 0 };

	//Container for all sound requests.
	StaticArray<DynamicArray<SoundRequest>, 2> soundRequestBuffers;

	/*
	*	Updates the sound system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

};