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
	constexpr uint32 studioInitFlags{ FMOD_STUDIO_NORMAL };
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

	banks.EmplaceSlow(newBank);
}

/*
*	Updates the sound system asynchronously.
*/
void SoundSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Update the FMOD Studio System.
	studioSystem->update();
}