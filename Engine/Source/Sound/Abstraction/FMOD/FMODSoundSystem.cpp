#if defined(CATALYST_SOUND_FMOD)
//Header file.
#include <Systems/SoundSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Multithreading.
#include <Multithreading/Task.h>

//Sound.
#include <Sound/Abstraction/FMOD/FMODAbstractionUtilities.h>

//Systems.
#include <Systems/TaskSystem.h>

namespace FMODSoundSystemConstants
{
	constexpr FMOD_INITFLAGS INITIALIZATION_FLAGS{ FMOD_INIT_STREAM_FROM_UPDATE | FMOD_INIT_MIX_FROM_UPDATE | FMOD_INIT_3D_RIGHTHANDED
#if FMOD_DEBUGGING
													| FMOD_INIT_PROFILE_ENABLE
#endif
	};
	constexpr FMOD_STUDIO_INITFLAGS STUDIO_INITIALIZATION_FLAGS{ FMOD_STUDIO_INIT_SYNCHRONOUS_UPDATE | FMOD_STUDIO_INIT_LOAD_FROM_UPDATE };
	constexpr int32 MAXIMUM_NUMBER_OF_CHANNELS{ 2'048 + 1'024 + 512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1 };
}

namespace FMODSoundSystemData
{
	//The update task.
	Task _UpdateTask;

	//The studio system.
	FMOD::Studio::System *RESTRICT _System{ nullptr };

	//The low level system.
	FMOD::System *RESTRICT _LowLevelSystem{ nullptr };
}

namespace FMODSoundSystemLogic
{

	/*
	*	Updates the listener position.
	*/
	void UpdateListenerPosition() NOEXCEPT
	{
		//Get the perceiver position, forward vector and up vector.
		const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };
		const Vector3<float> perceiverForwardVector{ Perceiver::Instance->GetForwardVector() };
		const Vector3<float> perceiverUpVector{ Perceiver::Instance->GetUpVector() };

		FMOD_3D_ATTRIBUTES listenerAttributes;

		listenerAttributes.position.x = perceiverPosition._X;
		listenerAttributes.position.y = perceiverPosition._Y;
		listenerAttributes.position.z = perceiverPosition._Z;

		listenerAttributes.velocity.x = 0.0f;
		listenerAttributes.velocity.y = 0.0f;
		listenerAttributes.velocity.z = 0.0f;

		listenerAttributes.forward.x = perceiverForwardVector._X;
		listenerAttributes.forward.y = perceiverForwardVector._Y;
		listenerAttributes.forward.z = perceiverForwardVector._Z;

		listenerAttributes.up.x = perceiverUpVector._X;
		listenerAttributes.up.y = perceiverUpVector._Y;
		listenerAttributes.up.z = perceiverUpVector._Z;

		FMOD_ERROR_CHECK(FMODSoundSystemData::_System->setListenerAttributes(0, &listenerAttributes));
	}

}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set up the update task.
	FMODSoundSystemData::_UpdateTask._Function = [](void *const RESTRICT)
	{
		//Update the studio system.
		FMOD_ERROR_CHECK(FMODSoundSystemData::_System->update());
	};
	FMODSoundSystemData::_UpdateTask._Arguments = nullptr;

	//Create the studio system.
	FMOD_ERROR_CHECK(FMOD::Studio::System::create(&FMODSoundSystemData::_System));

	//Retrieve the low level system.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->getLowLevelSystem(&FMODSoundSystemData::_LowLevelSystem));

	//Set the dsp buffer size.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_LowLevelSystem->setDSPBufferSize(configuration._MixingBufferSize, configuration._NumberOfMixingBuffers));

	//Set the number of software channels.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_LowLevelSystem->setSoftwareChannels(configuration._MaximumNumberOfChannels));

	//Initialize the studio system.
	FMOD_ERROR_CHECK(FMODSoundSystemData::_System->initialize(	FMODSoundSystemConstants::MAXIMUM_NUMBER_OF_CHANNELS,
																FMODSoundSystemConstants::STUDIO_INITIALIZATION_FLAGS,
																FMODSoundSystemConstants::INITIALIZATION_FLAGS,
																nullptr));
}

/*
*	Updates the sound system during the post update phase.
*/
void SoundSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Wait for the update task to finish.
	FMODSoundSystemData::_UpdateTask.WaitFor();

	//Update the listener position.
	FMODSoundSystemLogic::UpdateListenerPosition();

	//Execute the update task.
	TaskSystem::Instance->ExecuteTask(&FMODSoundSystemData::_UpdateTask);
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
*	Creates a sound instance based on the given sound description.
*/
void SoundSystem::CreateSoundInstance(const SoundDescriptionHandle description, SoundInstanceHandle *const RESTRICT instance) NOEXCEPT
{
	//Create the event instance.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventDescription *const RESTRICT>(description)->createInstance(reinterpret_cast<FMOD::Studio::EventInstance **const RESTRICT>(instance)));
}

/*
*	Destroys a sound instance as soon as the sound instance has stopped playing.
*/
void SoundSystem::DestroySoundInstance(const SoundInstanceHandle instance) NOEXCEPT
{
	//Release the instance.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->release());
}

/*
*	Sets the playback position in seconds for a sound instance.
*/
void SoundSystem::SetPlaybackPosition(const float position, const SoundInstanceHandle instance) NOEXCEPT
{
	//Set the playback position.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->setTimelinePosition(static_cast<int32>(position * 1'000.0f)));
}

/*
*	Plays a sound instance.
*/
void SoundSystem::Play(const SoundInstanceHandle instance) NOEXCEPT
{
	//Start playing the instance.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->start());
}

/*
*	Stops a sound instance.
*/
void SoundSystem::Stop(const SoundInstanceHandle instance) NOEXCEPT
{
	//Stop playing the instance.
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->stop(FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT));
}

/*
*	Returns the parameter index of a parameter of the given sound instance.
*/
uint32 SoundSystem::GetParameterIndex(const char *const RESTRICT parameter, const SoundInstanceHandle instance) NOEXCEPT
{
	//Retrieve the parameter instance.
	FMOD::Studio::ParameterInstance *RESTRICT parameterInstance;
	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->getParameter(parameter, &parameterInstance));

	//Retrieve the parameter description.
	FMOD_STUDIO_PARAMETER_DESCRIPTION parameterDescription;
	FMOD_ERROR_CHECK(parameterInstance->getDescription(&parameterDescription));

	//Return the parameter index.
	return static_cast<uint32>(parameterDescription.index);
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
*	Sets the position of a sound instance.
*/
void SoundSystem::SetPosition(const SoundInstanceHandle instance, const Vector3<float> &position) NOEXCEPT
{
	FMOD_3D_ATTRIBUTES attributes;

	attributes.position.x = position._X;
	attributes.position.y = position._Y;
	attributes.position.z = position._Z;

	attributes.velocity.x = 0.0f;
	attributes.velocity.y = 0.0f;
	attributes.velocity.z = 0.0f;

	attributes.forward.x = 0.0f;
	attributes.forward.y = 0.0f;
	attributes.forward.z = -1.0f;

	attributes.up.x = 0.0f;
	attributes.up.y = 1.0f;
	attributes.up.z = 0.0f;

	FMOD_ERROR_CHECK(static_cast<FMOD::Studio::EventInstance *const RESTRICT>(instance)->set3DAttributes(&attributes));
}

#endif