#if defined(CATALYST_SOUND_FMOD)
//Header file.
#include <Systems/SoundSystem.h>

//Sound.
#include <Sound/Abstraction/FMOD/FMODCore.h>

namespace FMODSoundSystemConstants
{
	constexpr FMOD_INITFLAGS INITIALIZATION_FLAGS{ FMOD_INIT_NORMAL };
	constexpr FMOD_STUDIO_INITFLAGS STUDIO_INITIALIZATION_FLAGS{ FMOD_STUDIO_INIT_NORMAL };
	constexpr int32 MAXIMUM_NUMBER_OF_CHANNELS{ 512 };
}

namespace FMODSoundSystemData
{
	//The studio system.
	FMOD::Studio::System *RESTRICT _System{ nullptr };
}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize() NOEXCEPT
{
	//Create the studio system.
	FMOD_ERROR_CHECK(FMOD::Studio::System::create(&FMODSoundSystemData::_System));

	//Initialize the studio system.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->initialize(	FMODSoundSystemConstants::MAXIMUM_NUMBER_OF_CHANNELS,
																FMODSoundSystemConstants::STUDIO_INITIALIZATION_FLAGS,
																FMODSoundSystemConstants::INITIALIZATION_FLAGS,
																nullptr));
}

/*
*	Updates the sound system.
*/
void SoundSystem::Update(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the studip system.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->update());
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{
	//Release the studio system.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->release());
}

/*
*	Creates a sound bank.
*/
void SoundSystem::CreateSoundBank(const SoundBankData &data, SoundBankHandle *const RESTRICT soundBank) NOEXCEPT
{
	//Load the bank.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->loadBankMemory(	reinterpret_cast<const char *const RESTRICT>(data._Data.Data()),
																	static_cast<int32>(data._Data.Size()),
																	FMOD_STUDIO_LOAD_MEMORY_MODE::FMOD_STUDIO_LOAD_MEMORY,
																	FMOD_STUDIO_LOAD_BANK_NORMAL,
																	reinterpret_cast<FMOD::Studio::Bank **const RESTRICT>(soundBank)));
}
#endif