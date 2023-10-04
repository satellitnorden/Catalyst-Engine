#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Sound/SoundSubSystemWASAPI.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/SoundSystem.h>

//Macros.
#define SAFE_RELEASE(POINTER) if (POINTER) { POINTER->Release(); POINTER = nullptr; }

//Sound sub system WASAPI constants.
namespace SoundSubSystemWASAPIConstants
{
	//List of desired bit depths.
	constexpr uint8 DESIRED_BIT_DEPTHS[]
	{
		16,
		32,
		8
	};

	//List of desired sample rates.
	constexpr float32 DESIRED_SAMPLE_RATES[]
	{
		44'100.0f,
		48'000.0f
	};

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

//Sound sub system WASAPI logic.
namespace SoundSubSystemWASAPILogic
{

	/*
	*	Calculates the optimal device period.
	*/
	NO_DISCARD REFERENCE_TIME CalculateOptimalDevicePeriod(const DWORD sample_rate) NOEXCEPT
	{
		return static_cast<REFERENCE_TIME>(	/* Seconds */static_cast<float32>(SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) / static_cast<float32>(sample_rate)
											/* Milliseconds */ * 1'000.0f
											/* Nanoseconds */ * 1'000'000.0f
											/* REFERENCE_TIME (100 nanoseconds) */ / 100.0f);
	}

}

/*
*	Initializes this sound sub system.
*/
void SoundSubSystemWASAPI::Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT
{
	//Set up the thread.
	_Thread.SetFunctionWithArguments
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<SoundSubSystemWASAPI *const RESTRICT>(arguments)->Update();
		},
		this
	);
	_Thread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_Thread.SetName("Sound Sub System WASAPI Thread");
#endif
	//Launch the thread!
	_Thread.Launch();
}

/*
*	Terminates this sound sub system.
*/
void SoundSubSystemWASAPI::Terminate() NOEXCEPT
{
	//Wait for the thread to finish.
	_Thread.Join();

	//Release the audio end point.
	SAFE_RELEASE(_AudioEndpoint);

	//Release the audio client.
	SAFE_RELEASE(_AudioClient);

	//Release the audio render client.
	SAFE_RELEASE(_AudioRenderClient);
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
NO_DISCARD float32 SoundSubSystemWASAPI::GetAudioLatency() const NOEXCEPT
{
	//Ask the audio client for the stream latency.
	REFERENCE_TIME stream_latency;

	if (_AudioClient->GetStreamLatency(&stream_latency) == S_OK)
	{
		//Hard-coded 30 ms adjustment here.
		return static_cast<float32>(stream_latency) / 10'000.0f + 30.0f;
	}

	else
	{
		//Guess?
		return 30.0f;
	}
}

//The update function.
void SoundSubSystemWASAPI::Update() NOEXCEPT
{
	//Define macros.
#define HANDLE_ERROR(FUNCTION) if (FAILED(FUNCTION)) { LOG_ERROR("Sound system failted to initialize!, %s failed!", #FUNCTION); goto CLEANUP; }

	//Define local variables.
	IMMDeviceEnumerator* RESTRICT device_enumerator{ nullptr };
	const WAVEFORMATEX* RESTRICT chosen_mix_format{ nullptr };
	REFERENCE_TIME default_device_period{ 0 };
	REFERENCE_TIME minimum_device_period{ 0 };
	REFERENCE_TIME optimal_device_period{ 0 };
	uint32 current_padding{ 0 };
	uint32 number_of_samples_available{ 0 };
	BYTE* RESTRICT buffer_data{ nullptr };

	//Initialize the COM library.
	HANDLE_ERROR(CoInitialize(nullptr));

	//Create the device enumerator.
	HANDLE_ERROR(CoCreateInstance(	SoundSubSystemWASAPIConstants::MMDeviceEnumerator_CLSID,
									nullptr,
									CLSCTX_ALL,
									SoundSubSystemWASAPIConstants::IMMDeviceEnumerator_IID,
									reinterpret_cast<void* RESTRICT* const RESTRICT>(&device_enumerator)));

	//Retrieve the default audio endpoint.
	HANDLE_ERROR(	device_enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
					ERole::eConsole,
					&_AudioEndpoint));

	//Activated the chosen audio endpoint and retrieve the audio client.
	HANDLE_ERROR(_AudioEndpoint->Activate(	SoundSubSystemWASAPIConstants::IAudioClient_IID,
											CLSCTX_ALL,
											nullptr,
											reinterpret_cast<void *RESTRICT *const RESTRICT>(&_AudioClient)));

	//Check if any of the desired formats are supported.
	for (const WAVEFORMATEX &desired_mix_format : SoundSubSystemWASAPIConstants::DESIRED_MIX_FORMATS)
	{
		WAVEFORMATEX *RESTRICT closest_match_dummy{ nullptr };
		const HRESULT result{ _AudioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &desired_mix_format, &closest_match_dummy) };

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

		LOG_ERROR("Couldn't find a supported mix format!");

		WAVEFORMATEX* RESTRICT device_format{ nullptr };
		HANDLE_ERROR(_AudioClient->GetMixFormat(&device_format));

		if (device_format)
		{
			LOG_INFORMATION("Device format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", device_format->nChannels, device_format->nSamplesPerSec, device_format->wBitsPerSample);

			CoTaskMemFree(device_format);
		}

		goto CLEANUP;
	}

	LOG_INFORMATION("Chosen mix format - nChannels: %i - nSamplesPerSec: %i - wBitsPerSample: %i", chosen_mix_format->nChannels, chosen_mix_format->nSamplesPerSec, chosen_mix_format->wBitsPerSample);

	//Retrieve the default and minimum device period.
	HANDLE_ERROR(_AudioClient->GetDevicePeriod(&default_device_period, &minimum_device_period));

	//Calculate the optimal device period.
	optimal_device_period = SoundSubSystemWASAPILogic::CalculateOptimalDevicePeriod(chosen_mix_format->nSamplesPerSec);

	//Initialize the audio client.
	HANDLE_ERROR(_AudioClient->Initialize(	AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
											0,
											CatalystBaseMath::Maximum<REFERENCE_TIME>(minimum_device_period, optimal_device_period),
											0,
											chosen_mix_format,
											nullptr));

	//Retrieve the buffer size.
	HANDLE_ERROR(_AudioClient->GetBufferSize(&_BufferSize));

	//Retrieve the audio render client.
	HANDLE_ERROR(_AudioClient->GetService(	SoundSubSystemWASAPIConstants::IAudioRenderClient_IID,
											reinterpret_cast<void* RESTRICT* const RESTRICT>(&_AudioRenderClient)));

	//Set the number of channels.
	_NumberOfChannels = static_cast<uint8>(chosen_mix_format->nChannels);

	//Set the sample rate.
	_SampleRate = static_cast<float32>(chosen_mix_format->nSamplesPerSec);

	//Set the sound format.
	switch (chosen_mix_format->wBitsPerSample)
	{
		case 8:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_8_BIT;

			break;
		}

		case 16:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_16_BIT;

			break;
		}

		case 32:
		{
			_SoundFormat = SoundFormat::SIGNED_INTEGER_32_BIT;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//The WASAPI sound sub system is successfully initialized!
	_Initialized = true;

	//Initialize the mixing buffers.
	_SoundSystem->InitializeMixingBuffers(SoundSystem::DEFAULT_NUMBER_OF_MIXING_BUFFERS, SoundSystem::DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER);

	//Start playing.
	HANDLE_ERROR(_AudioClient->Start());

	//Log.
	LOG_INFORMATION("Sound system successfully initialized!");

	//Main update loop.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Remember the start of the update.
		const std::chrono::steady_clock::time_point start_of_update{ std::chrono::steady_clock::now() };

		//Retrieve the current padding.
		HANDLE_ERROR(_AudioClient->GetCurrentPadding(&current_padding));

		//Calculate the number of samples available.
		number_of_samples_available = _BufferSize - current_padding;

		if (number_of_samples_available > 0)
		{
			//Retrieve the buffer.
			HANDLE_ERROR(_AudioRenderClient->GetBuffer(number_of_samples_available, &buffer_data));

			//Do the update.
			_SoundSystem->SoundCallback(static_cast<float32>(chosen_mix_format->nSamplesPerSec),
										_SoundFormat,
										static_cast<uint8>(chosen_mix_format->nChannels),
										number_of_samples_available,
										buffer_data);

			//Release the buffer.
			HANDLE_ERROR(_AudioRenderClient->ReleaseBuffer(number_of_samples_available, 0));
		}

		else
		{
			/*
			//Sleep approximately until the next buffer needs writing.
			const float64 milliseconds_to_sleep{ static_cast<float64>(WindowsSoundSystemData::_BufferSize / 2) / static_cast<float64>(GetSampleRate()) * 1'000.0 };
			const std::chrono::steady_clock::time_point next_update{ start_of_update + std::chrono::nanoseconds(static_cast<uint64>(milliseconds_to_sleep * 1'000'000.0)) };
			std::this_thread::sleep_until(next_update);
			*/
			//Yield the current thread.
			Concurrency::CurrentThread::Yield();
		}
	}

	//Stop playing.
	HANDLE_ERROR(_AudioClient->Stop());

CLEANUP:

	//Release the local variables.
	SAFE_RELEASE(device_enumerator);

	//Uninitialize the COM library.
	CoUninitialize();

	//Undefine macors.
#undef HANDLE_ERROR
}

//Undef macros.
#undef SAFE_RELEASE
#endif