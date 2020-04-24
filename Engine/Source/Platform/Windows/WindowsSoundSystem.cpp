#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

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
class WindowsCatalystSoundSystemLogic final
{

public:

	/*
	*	Updates the Windows Catalyst sound system asynchronously.
	*/
	FORCE_INLINE static void AsynchronousUpdate(void(*audio_callback)(const float32, const uint8, const uint8, const uint32, void *const RESTRICT)) NOEXCEPT
	{
		
	}

};

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set up the thread.
	WindowsCatalystSoundSystemData::_Thread.SetFunction([]()
	{
		SoundSystem::Instance->AsynchronousUpdate();
	});
	WindowsCatalystSoundSystemData::_Thread.SetPriority(Thread::Priority::NORMAL);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	WindowsCatalystSoundSystemData::_Thread.SetName("Windows Catalyst sound system");
#endif

	//Launch the thread!
	WindowsCatalystSoundSystemData::_Thread.Launch();
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
*	The asynchronous update function.
*/
void SoundSystem::AsynchronousUpdate() NOEXCEPT
{
	//Define constants.
	constexpr REFERENCE_TIME TIME_TO_REQUEST_IN_MILLISECONDS{ 32 };
	constexpr REFERENCE_TIME TIME_TO_REQUEST_IN_ACTUAL_TIME{ TIME_TO_REQUEST_IN_MILLISECONDS * 10 * 1'000 };

	//Define macros.
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { ASSERT(false, "Windows Catalyst sound system couldn't be initialized!"); goto CLEANUP; }

	//Define local variables.
	IMMDeviceEnumerator* RESTRICT device_enumerator{ nullptr };
	WAVEFORMATEX mix_format{ };
	uint32 buffer_size{ 0 };
	REFERENCE_TIME default_device_period{ 0 };
	REFERENCE_TIME minimum_device_period{ 0 };
	uint32 current_padding{ 0 };
	uint32 number_of_samples_available{ 0 };
	BYTE *RESTRICT buffer_data{ nullptr };

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	WindowsGUIDs::MMDeviceEnumerator_CLSID,
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

	//Set up the mix format.
	mix_format.wFormatTag = WAVE_FORMAT_PCM;
	mix_format.nChannels = 2;
	mix_format.nSamplesPerSec = 48000;
	mix_format.nAvgBytesPerSec = 48000 * 2 * 16 / 8;
	mix_format.nBlockAlign = 2 * 16 / 8;
	mix_format.wBitsPerSample = 16;

	//Retrieve the default and minimum device period.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetDevicePeriod(&default_device_period, &minimum_device_period));

	//Initialize the audio client.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
																			0,
																			TIME_TO_REQUEST_IN_ACTUAL_TIME,
																			0,
																			&mix_format,
																			nullptr));

	//Retrieve the buffer size.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetBufferSize(&buffer_size));

	//Retrieve the audio render client.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetService(	WindowsGUIDs::IAudioRenderClient_IID,
																			reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsCatalystSoundSystemData::_AudioRenderClient)));

	//The Windows Catalyst sound system is successfully initialized!
	WindowsCatalystSoundSystemData::_Initialized = true;

	//Start playing.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->Start());

	//Main update loop.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Retrieve the current padding.
		HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->GetCurrentPadding(&current_padding));

		//Calculate the number of samples available.
		number_of_samples_available = buffer_size - current_padding;

		if (number_of_samples_available > 0)
		{
			//Retrieve the buffer.
			HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioRenderClient->GetBuffer(number_of_samples_available, &buffer_data));

			//Do the update.
			SoundCallback(	static_cast<float32>(mix_format.nSamplesPerSec),
							static_cast<uint8>(mix_format.wBitsPerSample),
							static_cast<uint8>(mix_format.nChannels),
							number_of_samples_available,
							buffer_data);

			//Release the buffer.
			HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			Concurrency::CurrentThread::Yield();
		}
	}

	//Stop playing.
	HANDLE_ERROR(WindowsCatalystSoundSystemData::_AudioClient->Stop());

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}
#endif