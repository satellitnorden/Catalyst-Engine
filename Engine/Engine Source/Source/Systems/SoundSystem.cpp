//Header file.
#include <Systems/SoundSystem.h>

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

	FMOD_ERROR_CHECK(studioSystem->initialize(32, studioInitFlags, initFlags, nullptr));
}

/*
*	Updates the sound system synschronously.
*/
void SoundSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the asynchronous update to finish.
	updateSemaphore.WaitFor();

	//Update the current asynchronous sound system buffer.
	currentAsynchronousSoundSystemBuffer = currentSynchronousSoundSystemBuffer;

	//Update the current synchronous sound system buffer.
	currentSynchronousSoundSystemBuffer ^= 0x00000001;

	//Clear the current synchronous sound request buffer.
	soundRequestBuffers[currentSynchronousSoundSystemBuffer].ClearFast();

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
*	Loads an FMOD bank into memory.
*/
void SoundSystem::LoadBank(const char *const RESTRICT filePath) NOEXCEPT
{
	//Load the bank into memory.
	FMOD::Studio::Bank *RESTRICT newBank;
	FMOD_ERROR_CHECK(studioSystem->loadBankFile(filePath, FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank));

	newBank->loadSampleData();

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
*	Submits a sound request.
*/
void SoundSystem::SubmitSoundRequest(const SoundRequest &newSoundRequest) NOEXCEPT
{
	//Submit this sound request into the current sound system buffer.
	soundRequestBuffers[currentSynchronousSoundSystemBuffer].EmplaceSlow(newSoundRequest);
}

/*
*	Updates the sound system asynchronously.
*/
void SoundSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Go through all the sound requests and trigger them.
	for (const SoundRequest &soundRequest : soundRequestBuffers[currentAsynchronousSoundSystemBuffer])
	{
		FMOD::Studio::EventInstance *RESTRICT eventInstance;
 		FMOD_ERROR_CHECK(soundRequest.eventDescription->createInstance(&eventInstance));

		eventInstance->start();
	}

	//Update the FMOD Studio System.
	studioSystem->update();
}