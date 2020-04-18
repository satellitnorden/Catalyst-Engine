#if defined(CATALYST_PLATFORM_WINDOWS) && defined(CATALYST_SOUND_CATALYST)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/Native/SoundResourcePlayer.h>

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

	//The queued sounds.
	DynamicArray<SoundResourcePlayer> _QueuedSounds;

	//The queued sounds lock.
	Spinlock _QueuedSoundsLock;

	//The playing sounds.
	DynamicArray<SoundResourcePlayer> _PlayingSounds;

}

//Windows Catalyst sound system logic.
class WindowsCatalystSoundSystemLogic final
{

public:

	/*
	*	Updates the Windows Catalyst sound system asynchronously.
	*/
	FORCE_INLINE static void AsynchronousUpdate() NOEXCEPT
	{
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
																				minimum_device_period,
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
				Update(	static_cast<float32>(mix_format.nSamplesPerSec),
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

private:

	/*
	*	Updates the Windows Catalyst sound system.
	*/
	FORCE_INLINE static void Update(const float32 sample_rate,
									const uint8 bits_per_sample,
									const uint8 number_of_channels,
									const uint32 number_of_samples,
									BYTE *const RESTRICT buffer_data) NOEXCEPT
	{
		//Add the queued sounds to the playing sounds.
		WindowsCatalystSoundSystemData::_QueuedSoundsLock.Lock();

		for (const SoundResourcePlayer& queued_sound : WindowsCatalystSoundSystemData::_QueuedSounds)
		{
			WindowsCatalystSoundSystemData::_PlayingSounds.Emplace(queued_sound);
		}

		WindowsCatalystSoundSystemData::_QueuedSounds.Clear();

		WindowsCatalystSoundSystemData::_QueuedSoundsLock.Unlock();

		//Write all samples.
		uint64 sample_index{ 0 };

		for (uint32 i{ 0 }; i < number_of_samples; ++i)
		{
			for (uint8 j{ 0 }; j < number_of_channels; ++j)
			{
				//Calculate the current sample.
				float32 current_sample{ 0.0f };

				for (SoundResourcePlayer &playing_sound : WindowsCatalystSoundSystemData::_PlayingSounds)
				{
					current_sample += playing_sound.NextSample(j);
				}

				current_sample *= 0.5f;

				//Write the current value.
				switch (bits_per_sample)
				{
					case 8:
					{
						const int8 converted_sample{ static_cast<int8>(current_sample * static_cast<float32>(INT8_MAXIMUM)) };

						reinterpret_cast<int8 *const RESTRICT>(buffer_data)[sample_index++] = converted_sample;

						break;
					}

					case 16:
					{
						const int16 converted_sample{ static_cast<int16>(current_sample * static_cast<float32>(INT16_MAXIMUM)) };

						reinterpret_cast<int16 *const RESTRICT>(buffer_data)[sample_index++] = converted_sample;

						break;
					}

					case 32:
					{
						const int32 converted_sample{ static_cast<int32>(current_sample * static_cast<float32>(INT32_MAXIMUM)) };

						reinterpret_cast<int32 *const RESTRICT>(buffer_data)[sample_index++] = converted_sample;

						break;
					}

					default:
					{
						ASSERT(false, "Unhandled case!");

						break;
					}
				}
			}

			//Advance all playing sounds.
			for (SoundResourcePlayer& playing_sound : WindowsCatalystSoundSystemData::_PlayingSounds)
			{
				playing_sound.Advance();
			}
		}
		
		//Remove any inactive sounds.
		for (uint64 i{ 0 }; i < WindowsCatalystSoundSystemData::_PlayingSounds.Size();)
		{
			if (!WindowsCatalystSoundSystemData::_PlayingSounds[i].IsActive())
			{
				WindowsCatalystSoundSystemData::_PlayingSounds.EraseAt(i);
			}

			else
			{
				++i;
			}
		}
	}

};

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Set up the thread.
	WindowsCatalystSoundSystemData::_Thread.SetFunction(WindowsCatalystSoundSystemLogic::AsynchronousUpdate);
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
*	Plays a sound.
*/
void SoundSystem::PlaySound(const ResourcePointer<SoundResource> resource) NOEXCEPT
{
	//Set up the sound resource player.
	SoundResourcePlayer sound_resource_player;

	sound_resource_player.SetSoundResource(resource);

	//Add it to the queued sounds.
	WindowsCatalystSoundSystemData::_QueuedSoundsLock.Lock();
	WindowsCatalystSoundSystemData::_QueuedSounds.Emplace(sound_resource_player);
	WindowsCatalystSoundSystemData::_QueuedSoundsLock.Unlock();
}
#endif