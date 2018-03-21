//Header file.
#include <Systems/SoundSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/Sound3DEntity.h>

//Multithreading.
#include <Multithreading/Task.h>

//Sound.
#include <Sound/SoundCore.h>

//Systems.
#include <Systems/TaskSystem.h>

//Define the system.
DEFINE_SYSTEM(SoundSystem);

/*
*	Default constructor.
*/
SoundSystem::SoundSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
SoundSystem::~SoundSystem() NOEXCEPT
{

}

/*
*	Initializes the sound system.
*/
void SoundSystem::InitializeSystem() NOEXCEPT
{
	//Create the FMOD Studio System.
	FMOD_ERROR_CHECK(FMOD::Studio::System::create(&studioSystem, FMOD_VERSION));

	//Initialize the FMOD Studio System.
#if !defined(CATALYST_FINAL)
	constexpr uint32 studioInitFlags{ FMOD_STUDIO_INIT_LIVEUPDATE };
	constexpr uint32 initFlags{ FMOD_INIT_PROFILE_ENABLE };
#else
	constexpr uint32 studioInitFlags{ FMOD_STUDIO_INIT_NORMAL };
	constexpr uint32 initFlags{ FMOD_INIT_NORMAL };
#endif

	FMOD_ERROR_CHECK(studioSystem->initialize(256, studioInitFlags, initFlags, nullptr));
}

/*
*	Updates the sound system synschronously.
*/
void SoundSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the asynchronous update to finish.
	updateSemaphore.WaitFor();

	//Update active listener synchronously.
	UpdateActiveListenerSynchronous();

	//Flip the sound system buffers.
	currentAsynchronousSoundSystemBuffer ^= 1;
	currentSynchronousSoundSystemBuffer ^= 1;

	//Clear the current synchronous sound request buffers.
	sound3DInitializationRequestBuffers[currentSynchronousSoundSystemBuffer].ClearFast();
	sound3DUpdatePositionRequestBuffers[currentSynchronousSoundSystemBuffer].ClearFast();

	//Execute the asynchronous update task.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		StaticCast<SoundSystem *const RESTRICT>(arguments)->UpdateSystemAsynchronous();
	}, this, &updateSemaphore));
}

/*
*	Releases the sound system.
*/
void SoundSystem::ReleaseSystem() NOEXCEPT
{
	//Unload all banks.
	for (FMOD::Studio::Bank *const RESTRICT bank : banks)
	{
		FMOD_ERROR_CHECK(bank->unload());
	}

	//Release the FMOD Studio System.
	FMOD_ERROR_CHECK(studioSystem->release());
}

/*
*	Sets the active listener.
*/
void SoundSystem::SetActiveListener(const CameraEntity *const RESTRICT newActiveListener) NOEXCEPT
{
	//Set the active listener.
	activeListenerData.activeListener = newActiveListener;
}

/*
*	Loads an FMOD bank into memory.
*/
void SoundSystem::LoadBank(const char *const RESTRICT filePath) NOEXCEPT
{
	//Load the bank into memory.
	FMOD::Studio::Bank *RESTRICT newBank;
	FMOD_ERROR_CHECK(studioSystem->loadBankFile(filePath, FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank));

	FMOD_ERROR_CHECK(newBank->loadSampleData());

	banks.EmplaceSlow(newBank);
}

/*
*	Given an event name, returns the event description.
*/
const EventDescription *const RESTRICT SoundSystem::GetEventDescription(const char *const RESTRICT eventName) NOEXCEPT
{
	static DynamicString eventPrefix{ "event:/" };
	DynamicString actualEventName{ eventPrefix + eventName };

	FMOD::Studio::EventDescription *RESTRICT eventDescription;
	FMOD_ERROR_CHECK(studioSystem->getEvent(actualEventName.CString(), &eventDescription));

	return eventDescription;
}

/*
*	Initializes a sound 3D entity.
*/
void SoundSystem::InitializeSound3DEntity(Sound3DEntity *const RESTRICT entity, const EventDescription *const RESTRICT eventDescription) NOEXCEPT
{
	//Record a sound 3D initialization request into the current synchronous buffer.
	sound3DInitializationRequestBuffers[currentSynchronousSoundSystemBuffer].EmplaceSlow(entity, eventDescription);
}

/*
*	Updates the position of a sound 3D entity.
*/
void SoundSystem::UpdateSound3DEntityPosition(Sound3DEntity *const RESTRICT entity, const Vector3 &newPosition) NOEXCEPT
{
	//Record a sound 3D update poisition request into the current synchronous buffer.
	sound3DUpdatePositionRequestBuffers[currentSynchronousSoundSystemBuffer].EmplaceSlow(entity, newPosition);
}

/*
*	Updates the active listener synchronously.
*/
void SoundSystem::UpdateActiveListenerSynchronous() NOEXCEPT
{
	if (activeListenerData.activeListener)
	{
		//Copy the active listener position.
		activeListenerData.activeListenerPosition = activeListenerData.activeListener->GetPosition();

		//Copy the active listener forward vector.
		activeListenerData.activeListenerForwardVector = activeListenerData.activeListener->GetForwardVector();

		//Copy the active listener up vector.
		activeListenerData.activeListenerUpVector = activeListenerData.activeListener->GetUpVector();
	}
}

/*
*	Updates the sound system asynchronously.
*/
void SoundSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Update the active listener asynchronously.
	UpdateActiveListenerAsynchronous();

	//Update the sound 3D initialization requests.
	UpdateSound3DInitializationRequests();

	//Update the sound 3D update position requests.
	UpdateSound3DUpdatePositionRequests();

	//Update the FMOD Studio System.
	FMOD_ERROR_CHECK(studioSystem->update());
}

/*
*	Updates the active listener asynchronously.
*/
void SoundSystem::UpdateActiveListenerAsynchronous() const NOEXCEPT
{
	//Set the 3D attributes of the active listener.
	FMOD_3D_ATTRIBUTES attributes;

	attributes.position.x = activeListenerData.activeListenerPosition.X;
	attributes.position.y = activeListenerData.activeListenerPosition.Y;
	attributes.position.z = -activeListenerData.activeListenerPosition.Z;

	attributes.velocity.x = 0.0f;
	attributes.velocity.y = 0.0f;
	attributes.velocity.z = 0.0f;

	attributes.forward.x = activeListenerData.activeListenerForwardVector.X;
	attributes.forward.y = activeListenerData.activeListenerForwardVector.Y;
	attributes.forward.z = -activeListenerData.activeListenerForwardVector.Z;

	attributes.up.x = activeListenerData.activeListenerUpVector.X;
	attributes.up.y = activeListenerData.activeListenerUpVector.Y;
	attributes.up.z = -activeListenerData.activeListenerUpVector.Z;

	FMOD_ERROR_CHECK(studioSystem->setListenerAttributes(0, &attributes));
}

/*
*	Updates the sound 3D initialization requests.
*/
void SoundSystem::UpdateSound3DInitializationRequests() NOEXCEPT
{
	//Go through all the sound 3D initialization requests and process.
	for (const Sound3DInitializationRequest &sound3DInitializationRequest : sound3DInitializationRequestBuffers[currentAsynchronousSoundSystemBuffer])
	{
		//Get the sound 3D component.
		Sound3DComponent &component{ ComponentManager::GetSound3DComponents()[sound3DInitializationRequest.entity->GetComponentsIndex()] };

		//Create the event instance.
		FMOD_ERROR_CHECK(sound3DInitializationRequest.eventDescription->createInstance(&component.eventInstance));

		//Start the event instance.
		FMOD_ERROR_CHECK(component.eventInstance->start());
	}
}

/*
*	Updates the sound 3D update position requests.
*/
void SoundSystem::UpdateSound3DUpdatePositionRequests() NOEXCEPT
{
	//Go through all the sound 3D update position requests and process them.
	for (const Sound3DUpdatePositionRequest &sound3DUpdatePositionRequest : sound3DUpdatePositionRequestBuffers[currentAsynchronousSoundSystemBuffer])
	{
		//Update the position of the sound 3D entity.
		Sound3DComponent &component{ ComponentManager::GetSound3DComponents()[sound3DUpdatePositionRequest.entity->GetComponentsIndex()] };

		FMOD_3D_ATTRIBUTES attributes;

		attributes.position.x = sound3DUpdatePositionRequest.position.X;
		attributes.position.y = sound3DUpdatePositionRequest.position.Y;
		attributes.position.z = -sound3DUpdatePositionRequest.position.Z;

		attributes.velocity.x = 0.0f;
		attributes.velocity.y = 0.0f;
		attributes.velocity.z = 0.0f;

		attributes.forward.x = 0.0f;
		attributes.forward.y = 0.0f;
		attributes.forward.z = -1.0f;

		attributes.up.x = 0.0f;
		attributes.up.y = 1.0f;
		attributes.up.z = 0.0f;

		FMOD_ERROR_CHECK(component.eventInstance->set3DAttributes(&attributes));
		FMOD_ERROR_CHECK(component.eventInstance->start());
	}
}