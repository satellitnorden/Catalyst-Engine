#if defined(CATALYST_PLATFORM_WINDOWS) && defined(CATALYST_SOUND_CATALYST)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Thread.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Windows GUID's.
namespace WindowsGUIDs
{
	constexpr CLSID MMDeviceEnumerator_CLSID{ __uuidof(MMDeviceEnumerator) };
	constexpr IID IAudioClient_IID{ __uuidof(IAudioClient) };
	constexpr IID IAudioRenderClient_IID{ __uuidof(IAudioRenderClient) };
	constexpr IID IMMDeviceEnumerator_IID{ __uuidof(IMMDeviceEnumerator) };
}

//Windows Catalyst sound system data.
namespace WindowsCatalystSoundSystemData
{
	//The thread.
	Thread _Thread;

	//Denotes if the Windows Catalyst sound system is initialized.
	bool _Initialized{ false };

	//The audio endpoint.
	IMMDevice *RESTRICT _AudioEndpoint{ nullptr };

	//The audio client.
	IAudioClient *RESTRICT _AudioClient{ nullptr };

	//The audio render client.
	IAudioRenderClient *RESTRICT _AudioRenderClient{ nullptr };
}

//Windows Catalyst sound system logic.
namespace WindowsCatalystSoundSystemLogic
{

	/*
	*	Updates the Windows Catalyst sound system asynchronously.
	*/
	FORCE_INLINE void AsynchronousUpdate() NOEXCEPT
	{
		//Define macros.
		#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { ASSERT(false, "Windows Catalyst sound system couldn't be initialized!"); goto CLEANUP; }

		//Define local variables.
		IMMDeviceEnumerator *RESTRICT device_enumerator{ nullptr };
		WAVEFORMATEX *RESTRICT mix_format{ nullptr };
		uint32 buffer_size{ 0 };

		//Initialize the COM library.
		HANDLE_ERROR(CoInitialize(nullptr));

		//Create the device enumerator.
		HANDLE_ERROR(	CoCreateInstance(WindowsGUIDs::MMDeviceEnumerator_CLSID,
						nullptr,
						CLSCTX_ALL,
						WindowsGUIDs::IMMDeviceEnumerator_IID,
						reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

		//Retrieve the default audio endpoint.
		HANDLE_ERROR(device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
																ERole::eConsole,
																&WindowsCatalystSoundSystemData::_AudioEndpoint));

		//Activated the chosen audio endpoint and retrieve the audio client.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioEndpoint->Activate(	WindowsGUIDs::IAudioClient_IID,
																				CLSCTX_ALL,
																				nullptr,
																				reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsCatalystSoundSystemData::_AudioClient)));

		//Retrieve the mix format.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetMixFormat(&mix_format));

		//Initialize the audio client.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
																				0,
																				0,
																				0,
																				mix_format,
																				nullptr));

		//Retrieve the buffer size.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetBufferSize(&buffer_size));

		//Retrieve the audio render client.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetService(	WindowsGUIDs::IAudioRenderClient_IID,
																				reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsCatalystSoundSystemData::_AudioRenderClient)));

		//The Windows Catalyst sound system is successfully initialized!
		WindowsCatalystSoundSystemData::_Initialized = true;

	CLEANUP:

		//Release the local variables.
		SAFE_RELEASE(device_enumerator);
		CoTaskMemFree(mix_format);

		//Uninitialize the COM library.
		CoUninitialize();

		//Undefine macors.
#undef HANDLE_ERROR
	}

}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set up the thread.
	WindowsCatalystSoundSystemData::_Thread.SetFunction(WindowsCatalystSoundSystemLogic::AsynchronousUpdate);
	WindowsCatalystSoundSystemData::_Thread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	WindowsCatalystSoundSystemData::_Thread.SetName("Windows Catalyst sound system");
#endif

	//Launch the thread!
	WindowsCatalystSoundSystemData::_Thread.Launch();
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
	//Wait for the thread to finish.
	WindowsCatalystSoundSystemData::_Thread.Join();

	//Release the audio end point.
	SAFE_RELEASE(WindowsCatalystSoundSystemData::_AudioEndpoint);

	//Release the audio client.
	SAFE_RELEASE(WindowsCatalystSoundSystemData::_AudioClient);

	//Release the audio render client.
	SAFE_RELEASE(WindowsCatalystSoundSystemData::_AudioRenderClient);
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	//Don't do anything if the Windows Catalyst sound system isn't initialized.
	if (!WindowsCatalystSoundSystemData::_Initialized)
	{
		return 0;
	}

	return 0;
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
uint32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	//Don't do anything if the Windows Catalyst sound system isn't initialized.
	if (!WindowsCatalystSoundSystemData::_Initialized)
	{
		return 0;
	}

	return 0;
}

/*
*	Registers a process sound function.
*	Doing this will leave all responsibility to the game to produce the sound output and bypass the sound system's own processing.
*/
void SoundSystem::RegisterProcessSoundFunction(const ProcessSoundFunction function) NOEXCEPT
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