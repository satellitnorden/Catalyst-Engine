#if defined(CATALYST_PLATFORM_WINDOWS) && defined(CATALYST_SOUND_CATALYST)
//Header file.
#include <Systems/SoundSystem.h>

//Windows.
#include <Functiondiscoverykeys_devpkey.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Windows GUID's.
namespace WindowsGUIDs
{
	constexpr IID IMMDeviceEnumerator_IID{ __uuidof(IMMDeviceEnumerator) };
	constexpr CLSID MMDeviceEnumerator_CLSID{ __uuidof(MMDeviceEnumerator) };
}

//Windows Catalyst sound system data.
namespace WindowsCatalystSoundSystemData
{
	//Denotes if the Windows Catalyst sound system is initialized.
	bool _Initialized{ false };

	//The audio endpoint.
	IMMDevice *RESTRICT _AudioEndpoint{ nullptr };
}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Define macros.
	#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { ASSERT(false, "Windows Catalyst sound system couldn't be initialized!"); goto CLEANUP; }

	//Create the device enumerator.
	IMMDeviceEnumerator *RESTRICT device_enumerator{ nullptr };

	HANDLE_ERROR(	CoCreateInstance(WindowsGUIDs::MMDeviceEnumerator_CLSID,
					nullptr,
					CLSCTX_ALL,
					WindowsGUIDs::IMMDeviceEnumerator_IID,
					reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

	//Retrieve the default audio endpoint.
	HANDLE_ERROR(device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &WindowsCatalystSoundSystemData::_AudioEndpoint));

	//The Windows Catalyst sound system is successfully initialized!
	WindowsCatalystSoundSystemData::_Initialized = true;

CLEANUP:

	//Release the device enumerator.
	SAFE_RELEASE(device_enumerator);

	//Undefine macors.
	#undef HANDLE_ERROR
}

/*
*	Updates the sound system during the post update phase.
*/
void SoundSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Don't do anything if the Windows Catalyst sound system isn't initialized.
	if (!WindowsCatalystSoundSystemData::_Initialized)
	{
		return;
	}
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{
	//Release the audio end point.
	SAFE_RELEASE(WindowsCatalystSoundSystemData::_AudioEndpoint);
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
*	Returns the playback state of a sound instance.
*/
PlaybackState SoundSystem::GetPlaybackState(const SoundInstanceHandle instance) NOEXCEPT
{
	return PlaybackState::Stopped;
}

/*
*	Sets the position of a sound instance.
*/
void SoundSystem::SetPosition(const SoundInstanceHandle instance, const Vector3<float> &position) NOEXCEPT
{
	
}
#endif