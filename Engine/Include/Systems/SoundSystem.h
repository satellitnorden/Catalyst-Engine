#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

//Sound.
#include <Sound/SoundCore.h>

//Forward declarations.
class CameraEntity;
class Sound2DEntity;
class Sound3DEntity;
class SoundBank;
class SoundBankData;

class SoundSystem final
{

public:

	//Declare the singleton.
	DECLARE_SINGLETON(SoundSystem);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT { }

#if defined(CATALYST_WINDOWS)
	/*
	*	Given a string of an FMOD GUID, parse it and return it into a FMOD_GUID struct.
	*/
	static FMOD_GUID ParseGUID(const char *const RESTRICT string) NOEXCEPT;
#endif

	/*
	*	Initializes the sound system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the sound system synschronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Releases the sound system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Sets the active listener.
	*/
	void SetActiveListener(const CameraEntity *const RESTRICT newActiveListener) NOEXCEPT;

#if defined(CATALYST_WINDOWS)
	/*
	*	Loads a sound bank into memory.
	*/
	void LoadSoundBank(const SoundBankData &soundBankData, SoundBank &soundBank) NOEXCEPT;

	/*
	*	Given an event GUID, returns the event description.
	*/
	const FMOD::Studio::EventDescription *const RESTRICT GetEventDescription(const FMOD_GUID *const RESTRICT eventGuid) NOEXCEPT;

	/*
	*	Initializes a sound 2D entity.
	*/
	void InitializeSound2DEntity(Sound2DEntity *const RESTRICT entity, const FMOD::Studio::EventDescription *const RESTRICT eventDescription) NOEXCEPT;

	/*
	*	Initializes a sound 3D entity.
	*/
	void InitializeSound3DEntity(Sound3DEntity *const RESTRICT entity, const FMOD::Studio::EventDescription *const RESTRICT eventDescription) NOEXCEPT;

	/*
	*	Updates the position of a sound 3D entity.
	*/
	void UpdateSound3DEntityPosition(Sound3DEntity *const RESTRICT entity, const Vector3 &newPosition) NOEXCEPT;

private:

	/*
	*	Active listener data definition.
	*/
	class ActiveListenerData final
	{

	public:

		//Pointer the the active listener entity.
		const CameraEntity *RESTRICT activeListener{ nullptr };

		//The active listener position.
		Vector3 activeListenerPosition;

		//The active listener velocity.
		Vector3 activeListenerVelocity;

		//The active listener forward vector.
		Vector3 activeListenerForwardVector;

		//The active listener up vector.
		Vector3 activeListenerUpVector;

	};

	/*
	*	Sound 2D initialization request definition.
	*/
	class Sound2DInitializationRequest final
	{

	public:

		//The entity that likes to be initialized.
		Sound2DEntity * const RESTRICT entity;

		//The event description.
		const FMOD::Studio::EventDescription *const RESTRICT eventDescription;

		/*
		*	Constructor taking all values as arguments.
		*/
		Sound2DInitializationRequest(Sound2DEntity *const RESTRICT initialEntity, const FMOD::Studio::EventDescription *const RESTRICT initialEventDescription) NOEXCEPT
			:
			entity(initialEntity),
			eventDescription(initialEventDescription)
		{

		}

	};

	/*
	*	Sound 3D initialization request definition.
	*/
	class Sound3DInitializationRequest final
	{

	public:

		//The entity that likes to be initialized.
		Sound3DEntity *const RESTRICT entity;

		//The event description.
		const FMOD::Studio::EventDescription *const RESTRICT eventDescription;

		/*
		*	Constructor taking all values as arguments.
		*/
		Sound3DInitializationRequest(Sound3DEntity *const RESTRICT initialEntity, const FMOD::Studio::EventDescription *const RESTRICT initialEventDescription) NOEXCEPT
			:
			entity(initialEntity),
			eventDescription(initialEventDescription)
		{

		}

	};

	/*
	*	Sound 3D update position request definition.
	*/
	class Sound3DUpdatePositionRequest final
	{

	public:

		//The entity that likes to be moved.
		Sound3DEntity *const RESTRICT entity;

		//The new position of the sound 3D entity.
		Vector3 position;

		/*
		*	Constructor taking all values as arguments.
		*/
		Sound3DUpdatePositionRequest(Sound3DEntity *const RESTRICT initialEntity, const Vector3 &initialPosition) NOEXCEPT
			:
			entity(initialEntity),
			position(initialPosition)
		{

		}

	};

	//The fmod studio system.
	FMOD::Studio::System *RESTRICT studioSystem;

	//Container for all banks that is loaded at runtime.
	DynamicArray<FMOD::Studio::Bank *RESTRICT> banks;

	//The active listener data.
	ActiveListenerData activeListenerData;

	//Defines the current synchronous sound system buffer.
	uint8 currentSynchronousSoundSystemBuffer{ 0 };

	//Defines the current asynchronous sound system buffer.
	uint8 currentAsynchronousSoundSystemBuffer{ 1 };

	//Container for all sound 2D initialization requests.
	StaticArray<DynamicArray<Sound2DInitializationRequest>, 2> sound2DInitializationRequestBuffers;

	//Container for all sound 3D initialization requests.
	StaticArray<DynamicArray<Sound3DInitializationRequest>, 2> sound3DInitializationRequestBuffers;

	//Container for all sound 3D update position requests.
	StaticArray<DynamicArray<Sound3DUpdatePositionRequest>, 2> sound3DUpdatePositionRequestBuffers;

	/*
	*	Updates the active listener synchronously.
	*/
	void UpdateActiveListenerSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Updates the sound system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Updates the active listener asynchronously.
	*/
	void UpdateActiveListenerAsynchronous() const NOEXCEPT;

	/*
	*	Updates the sound 2D initialization requests.
	*/
	void UpdateSound2DInitializationRequests() NOEXCEPT;

	/*
	*	Updates the sound 3D initialization requests.
	*/
	void UpdateSound3DInitializationRequests() NOEXCEPT;

	/*
	*	Updates the sound 3D update position requests.
	*/
	void UpdateSound3DUpdatePositionRequests() NOEXCEPT;
#endif

};