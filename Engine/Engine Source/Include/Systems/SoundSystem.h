#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Sound.
#include <Sound/SoundCore.h>

//Forward declarations.
class Entity;
class Sound3DEntity;

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
	*	Initializes a sound 3D entity.
	*/
	void InitializeSound3DEntity(Sound3DEntity *const RESTRICT entity, const EventDescription *const RESTRICT eventDescription) NOEXCEPT;

private:

	/*
	*	Sound 3D initialization request definition.
	*/
	class Sound3DInitializationRequest final
	{

	public:

		//The entity that likes to be initialized.
		Sound3DEntity *const RESTRICT entity;

		//The event description.
		const EventDescription *const RESTRICT eventDescription;

		/*
		*	Constructor taking all values as arguments.
		*/
		Sound3DInitializationRequest(Sound3DEntity *const RESTRICT initialEntity, const EventDescription *const RESTRICT initialEventDescription) NOEXCEPT
			:
			entity(initialEntity),
			eventDescription(initialEventDescription)
		{

		}

	};

	//The update semaphore.
	Semaphore updateSemaphore{ SemaphoreCreationFlags::Signalled };

	//The fmod studio system.
	FMOD::Studio::System *RESTRICT studioSystem;

	//Container for all banks that is loaded at runtime.
	DynamicArray<FMOD::Studio::Bank *RESTRICT> banks;

	//The active listener.
	const Entity *RESTRICT activeListener;

	//Defines the current synchronous sound system buffer.
	uint8 currentSynchronousSoundSystemBuffer{ 0 };

	//Defines the current asynchronous sound system buffer.
	uint8 currentAsynchronousSoundSystemBuffer{ 1 };

	//Container for all sound 3D initialization requests.
	StaticArray<DynamicArray<Sound3DInitializationRequest>, 2> sound3DInitializationRequestBuffers;

	/*
	*	Updates the sound system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Updates the active listener.
	*/
	void UpdateActiveListener() const NOEXCEPT;

	/*
	*	Updates the sound 3D initialization requests.
	*/
	void UpdateSound3DInitializationRequests() NOEXCEPT;

};