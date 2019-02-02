#if defined(CATALYST_SOUND_FMOD)
//Header file.
#include <Systems/SoundSystem.h>

//Sound.
#include <Sound/Abstraction/FMOD/FMODAbstractionUtilities.h>

namespace FMODSoundSystemConstants
{
	constexpr FMOD_INITFLAGS INITIALIZATION_FLAGS{	FMOD_INIT_NORMAL
#if FMOD_DEBUGGING
													| FMOD_INIT_PROFILE_ENABLE
#endif
	};
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
void SoundSystem::CreateSoundBank(const SoundBankData &data, SoundBankHandle *const RESTRICT handle) NOEXCEPT
{
	//Load the bank.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->loadBankMemory(	reinterpret_cast<const char *const RESTRICT>(data._Data.Data()),
																	static_cast<int32>(data._Data.Size()),
																	FMOD_STUDIO_LOAD_MEMORY_MODE::FMOD_STUDIO_LOAD_MEMORY,
																	FMOD_STUDIO_LOAD_BANK_NORMAL,
																	reinterpret_cast<FMOD::Studio::Bank **const RESTRICT>(handle)));
}

/*
*	Given an ID, returns the sound description.
*/
void SoundSystem::GetSoundDescription(const char *const RESTRICT ID, SoundDescriptionHandle *const RESTRICT handle) NOEXCEPT
{
	//Parse the FMOD GUID.
	FMOD_GUID guid;
	FMOD_ERROR_CHECK(FMOD::Studio::parseID(ID, &guid));

	//Get the event.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->getEventByID(	&guid,
																	reinterpret_cast<FMOD::Studio::EventDescription **const RESTRICT>(handle)));
}

/*
*	Returns the playback state of a sound instance.
*/
PlaybackState SoundSystem::GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT
{
	//Get the FMOD playback state of the instance.
	FMOD_STUDIO_PLAYBACK_STATE playbackState;
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->getPlaybackState(&playbackState));

	//Return the native playback state.
	return FMODAbstractionUtilities::GetPlaybackState(playbackState);
}

/*
*	Sets the parameter on the given sound instance at the given index.
*	The index of a parameter might not always be the same as it appears in the sound editor.
*/
void SoundSystem::SetParameterAtIndex(const SoundInstanceHandle instance, const uint32 index, const float value) NOEXCEPT
{
	//Set the parameter at the given index.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->setParameterValueByIndex(static_cast<int32>(index), value));
}

/*
*	Plays a two dimensional sound.
*	Can supply an optional handle to the sound instance to keep track of the sound properties.
*	If not supplied, a temporary instance will be created that will be destroyed as soon as the sound has finished playing.
*/
void SoundSystem::PlayTwoDimensionalSound(const SoundDescriptionHandle description, SoundInstanceHandle *const RESTRICT instance) NOEXCEPT
{
	//Create the event instance.
	FMOD::Studio::EventInstance *RESTRICT temporaryInstance;
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventDescription *const RESTRICT>(description)->createInstance(&temporaryInstance));

	//Start playing the instance.
	FMOD_ERROR_CHECK(temporaryInstance->start());

	//Set the supplied handle to the instance, if it is valid.
	if (instance)
	{
		//If the handle is already pointing to a valid instance, make sure to release it.
		if (*instance)
		{
			FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(*instance)->release());
		}

		*instance = temporaryInstance;
	}

	else
	{
		//Instantly release the instance to make sure it is destroyed when it is stopped, since no handle to it is stored.
		FMOD_ERROR_CHECK(temporaryInstance->release());
	}
}
#endif