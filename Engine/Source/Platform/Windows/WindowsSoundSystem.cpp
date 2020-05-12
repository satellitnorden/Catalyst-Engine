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

//Windows sound system constants.
namespace WindowsSoundSystemConstants
{
	constexpr CLSID MMDeviceEnumerator_CLSID{ __uuidof(MMDeviceEnumerator) };
	constexpr IID IAudioClient_IID{ __uuidof(IAudioClient) };
	constexpr IID IAudioRenderClient_IID{ __uuidof(IAudioRenderClient) };
	constexpr IID IMMDeviceEnumerator_IID{ __uuidof(IMMDeviceEnumerator) };

	//List of desired mix formats.
	constexpr WAVEFORMATEX DESIRED_MIX_FORMATS[]
	{
		//Stereo, 44.100 Khz, 16-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 16 / 8,	//nAvgBytesPerSec
			2 * 16 / 8,			//nBlockAlign
			16					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 16-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 16 / 8,	//nAvgBytesPerSec
			2 * 16 / 8,			//nBlockAlign
			16					//wBitsPerSample
		},
		//Stereo, 44.100 Khz, 32-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 32 / 8,	//nAvgBytesPerSec
			2 * 32 / 8,			//nBlockAlign
			32					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 32-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 32 / 8,	//nAvgBytesPerSec
			2 * 32 / 8,			//nBlockAlign
			32					//wBitsPerSample
		},
		//Stereo, 44.100 Khz, 8-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			44100,				//nSamplesPerSec
			44100 * 2 * 8 / 8,	//nAvgBytesPerSec
			2 * 8 / 8,			//nBlockAlign
			8					//wBitsPerSample
		},
		//Stereo, 48.000 Khz, 8-Bit
		{
			WAVE_FORMAT_PCM,	//wFormatTag
			2,					//nChannels
			48000,				//nSamplesPerSec
			48000 * 2 * 8 / 8,	//nAvgBytesPerSec
			2 * 8 / 8,			//nBlockAlign
			8					//wBitsPerSample
		}
	};
}

//Windows sound system data.
namespace WindowsSoundSystemData
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

	//The sample rate.
	float32 _SampleRate;

}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize() NOEXCEPT
{
	//Set up the thread.
	WindowsSoundSystemData::_Thread.SetFunction([]()
	{
		SoundSystem::Instance->AsynchronousUpdate();
	});
	WindowsSoundSystemData::_Thread.SetPriority(Thread::Priority::NORMAL);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	WindowsSoundSystemData::_Thread.SetName("Windows Catalyst sound system");
#endif

	//Launch the thread!
	WindowsSoundSystemData::_Thread.Launch();
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{
	//Wait for the thread to finish.
	WindowsSoundSystemData::_Thread.Join();

	//Release the audio end point.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioEndpoint);

	//Release the audio client.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioClient);

	//Release the audio render client.
	SAFE_RELEASE(WindowsSoundSystemData::_AudioRenderClient);
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	//Don't do anything if the Windows Catalyst sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	return 0;
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
float32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	//Don't do anything if the Windows Catalyst sound system isn't initialized.
	if (!WindowsSoundSystemData::_Initialized)
	{
		return 0;
	}

	return WindowsSoundSystemData::_SampleRate;
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
	const WAVEFORMATEX *RESTRICT chosen_mix_format{ nullptr };
	uint32 buffer_size{ 0 };
	REFERENCE_TIME default_device_period{ 0 };
	REFERENCE_TIME minimum_device_period{ 0 };
	uint32 current_padding{ 0 };
	uint32 number_of_samples_available{ 0 };
	BYTE *RESTRICT buffer_data{ nullptr };

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	WindowsSoundSystemConstants::MMDeviceEnumerator_CLSID,
									nullptr,
									CLSCTX_ALL,
									WindowsSoundSystemConstants::IMMDeviceEnumerator_IID,
									reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

	//Retrieve the default audio endpoint.
	HANDLE_ERROR(device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
															ERole::eConsole,
															&WindowsSoundSystemData::_AudioEndpoint));

	//Activated the chosen audio endpoint and retrieve the audio client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioEndpoint->Activate(	WindowsSoundSystemConstants::IAudioClient_IID,
																	CLSCTX_ALL,
																	nullptr,
																	reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsSoundSystemData::_AudioClient)));

	//Check if any of the desired formats are supported.
	for (const WAVEFORMATEX &desired_mix_format : WindowsSoundSystemConstants::DESIRED_MIX_FORMATS)
	{
		WAVEFORMATEX *RESTRICT closest_match_dummy{ nullptr };
		const HRESULT result{ WindowsSoundSystemData::_AudioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &desired_mix_format, &closest_match_dummy) };

		if (closest_match_dummy)
		{
			CoTaskMemFree(closest_match_dummy);
		}

		if (result == S_OK)
		{
			chosen_mix_format = &desired_mix_format;

			break;
		}
	}

	//If none of the desired mix formats were supported, abort.
	if (!chosen_mix_format)
	{
		ASSERT(false, "None of the desired mix formats were supported!");

		goto CLEANUP;
	}

	//Retrieve the default and minimum device period.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetDevicePeriod(&default_device_period, &minimum_device_period));

	//Initialize the audio client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
																	0,
																	TIME_TO_REQUEST_IN_ACTUAL_TIME,
																	0,
																	chosen_mix_format,
																	nullptr));

	//Retrieve the buffer size.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetBufferSize(&buffer_size));

	//Retrieve the audio render client.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetService(	WindowsSoundSystemConstants::IAudioRenderClient_IID,
																	reinterpret_cast<void* RESTRICT* const RESTRICT>(&WindowsSoundSystemData::_AudioRenderClient)));

	//Set the sample rate.
	WindowsSoundSystemData::_SampleRate = static_cast<float32>(chosen_mix_format->nSamplesPerSec);

	//The Windows sound system is successfully initialized!
	WindowsSoundSystemData::_Initialized = true;

	//Start playing.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Start());

	//Main update loop.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Retrieve the current padding.
		HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->GetCurrentPadding(&current_padding));

		//Calculate the number of samples available.
		number_of_samples_available = buffer_size - current_padding;

		if (number_of_samples_available > 0)
		{
			//Retrieve the buffer.
			HANDLE_ERROR(WindowsSoundSystemData::_AudioRenderClient->GetBuffer(number_of_samples_available, &buffer_data));

			//Do the update.
			SoundCallback(	static_cast<float32>(chosen_mix_format->nSamplesPerSec),
							static_cast<uint8>(chosen_mix_format->wBitsPerSample),
							static_cast<uint8>(chosen_mix_format->nChannels),
							number_of_samples_available,
							buffer_data);

			//Release the buffer.
			HANDLE_ERROR(WindowsSoundSystemData::_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			Concurrency::CurrentThread::Yield();
		}
	}

	//Stop playing.
	HANDLE_ERROR(WindowsSoundSystemData::_AudioClient->Stop());

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}
#endif