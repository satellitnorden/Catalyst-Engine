#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Third party libraries.
#include <Third Party Libraries/fmod_studio.hpp>

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

private:

	//The update semaphore.
	Semaphore updateSemaphore{ SemaphoreCreationFlags::Signalled };

	//The fmod studio system.
	FMOD::Studio::System *RESTRICT studioSystem;

	//Container for all banks that is loaded at runtime.
	DynamicArray<FMOD::Studio::Bank *RESTRICT> banks;

	/*
	*	Updates the sound system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

};