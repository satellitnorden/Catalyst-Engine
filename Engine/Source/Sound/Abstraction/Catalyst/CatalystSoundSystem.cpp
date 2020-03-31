#if defined(CATALYST_SOUND_CATALYST)
//Header file.
#include <Systems/SoundSystem.h>

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	
}

/*
*	Updates the sound system during the post update phase.
*/
void SoundSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{

}

/*
*	Creates a sound bank.
*/
void SoundSystem::CreateSoundBank(const SoundBankData &data, SoundBankHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Given an ID, returns the sound description.
*/
void SoundSystem::GetSoundDescription(const char *const RESTRICT ID, SoundDescriptionHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Creates a sound instance based on the given sound description.
*/
void SoundSystem::CreateSoundInstance(const SoundDescriptionHandle description, SoundInstanceHandle *const RESTRICT instance) NOEXCEPT
{
	
}

/*
*	Destroys a sound instance as soon as the sound instance has stopped playing.
*/
void SoundSystem::DestroySoundInstance(const SoundInstanceHandle instance) NOEXCEPT
{

}

/*
*	Sets the playback position in seconds for a sound instance.
*/
void SoundSystem::SetPlaybackPosition(const float position, const SoundInstanceHandle instance) NOEXCEPT
{

}

/*
*	Plays a sound instance.
*/
void SoundSystem::Play(const SoundInstanceHandle instance) NOEXCEPT
{

}

/*
*	Stops a sound instance.
*/
void SoundSystem::Stop(const SoundInstanceHandle instance) NOEXCEPT
{

}

/*
*	Returns the parameter index of a parameter of the given sound instance.
*/
uint32 SoundSystem::GetParameterIndex(const char *const RESTRICT parameter, const SoundInstanceHandle instance) NOEXCEPT
{
	return 0;
}

/*
*	Returns the playback state of a sound instance.
*/
PlaybackState SoundSystem::GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT
{
	return PlaybackState::Stopped;
}

/*
*	Sets the parameter on the given sound instance at the given index.
*	The index of a parameter might not always be the same as it appears in the sound editor.
*/
void SoundSystem::SetParameterAtIndex(const SoundInstanceHandle instance, const uint32 index, const float value) NOEXCEPT
{
	
}

/*
*	Sets the position of a sound instance.
*/
void SoundSystem::SetPosition(const SoundInstanceHandle instance, const Vector3<float> &position) NOEXCEPT
{
	
}
#endif