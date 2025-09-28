//Header file.
#include <Systems/SoundSystem.h>

//File.
#include <File/Writers/WAVWriter.h>

//Profiling.
#include <Profiling/Profiling.h>

//Sound.
#include <Sound/SoundAssetPlayer.h>
#include <Sound/SoundSubSystemASIO.h>
#include <Sound/SoundSubSystemWASAPI.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Queued play sound request class definition.
*/
class QueuedPlaySoundRequest final
{

public:

	//The sound asset.
	AssetPointer<SoundAsset> _SoundAsset;

	//The gain.
	float32 _Gain;

	//The pan.
	float32 _Pan;

	//The start time.
	float32 _StartTime;

	//Denotes the playback rate.
	float32 _PlaybackRate;

	//Denotes if the sound is looping.
	bool _IsLooping;

	//The attack time.
	float32 _AttackTime;

	//The decay time.
	float32 _DecayTime;

	//The sustain gain.
	float32 _SustainGain;

	//The release time.
	float32 _ReleaseTime;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

	//The sound started callback.
	SoundStartedCallback _SoundStartedCallback;

	//The sound stopped callback.
	SoundStoppedCallback _SoundStoppedCallback;

	//The audio time tracker.
	Atomic<float64> *RESTRICT _AudioTimeTracker{ nullptr };

};

/*
*	Queued stop sound request class definition.
*/
class QueuedStopSoundRequest final
{

public:

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

};

/*
*	Playing sound class definition.
*/
class PlayingSound final
{

public:

	//The sound asset players.
	SoundAssetPlayer _SoundAssetPlayer;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

	//The sound stopped callback.
	SoundStoppedCallback _SoundStoppedCallback;

	//The audio time tracker.
	Atomic<float64> *RESTRICT _AudioTimeTracker{ nullptr };

};

//Sound system constants.
namespace SoundSystemConstants
{

	//The maximum number of queued master channel mix components.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS{ 8 };

	//The maximum number of queued play sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS{ 128 };

	//The maximum number of queued stop sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS{ 128 };

	//The maximum number of playing sounds.
	constexpr uint64 MAXIMUM_NUMBER_OF_PLAYING_SOUNDS{ 1'024 };

}

//Sound system data.
namespace SoundSystemData
{

	//The queued add master channel mix components.
	AtomicQueue<SoundMixComponent, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedAddMasterChannelSoundMixComponents;

	//The queued remove master channel mix components.
	AtomicQueue<uint64, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedRemoveMasterChannelSoundMixComponents;

	//The queued play sound requests.
	AtomicQueue<QueuedPlaySoundRequest, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedPlaySoundRequests;

	//The queued stop sounds.
	AtomicQueue<QueuedStopSoundRequest, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedStopSoundRequests;

	//The playing sounds.
	DynamicArray<PlayingSound> _PlayingSounds;

}

/*
*	Initializes the sound system.
*/
void SoundSystem::Initialize() NOEXCEPT
{
	//Copy the configuration.
	_Configuration = CatalystEngineSystem::Instance->GetProjectConfiguration()->_SoundConfiguration;

	//Reserve the appropriate memory to (hopefully) avoid memory allocations in mixing thread.
	SoundSystemData::_PlayingSounds.Reserve(SoundSystemConstants::MAXIMUM_NUMBER_OF_PLAYING_SOUNDS);

	//Launch the mixing thread.
	_MixingThread.SetFunction([]()
	{
		SoundSystem::Instance->Mix();
	});
	_MixingThread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_MixingThread.SetName("Sound System - Mixing Thread");
#endif

	_MixingThread.Launch();

	//Create the sub system.
	switch (_Configuration._SoundSubSystemType)
	{
		case SoundSubSystemType::WASAPI:
		case SoundSubSystemType::DEFAULT:
		{
			_SubSystem = new SoundSubSystemWASAPI();

			break;
		}

		case SoundSubSystemType::ASIO:
		{
			_SubSystem = new SoundSubSystemASIO();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the sound system.
	_SubSystem->_SoundSystem = this;

	//Initialize the sub system!
	{
		//If the audio device is set, find and set it!
		DynamicArray<AudioDevice> audio_devices;
		AudioDevice *RESTRICT audio_device{ nullptr };

		if (_Configuration._AudioDevice.Valid())
		{
			SoundSystem::Instance->QueryAudioDevices(&audio_devices);

			for (AudioDevice &queried_audio_device : audio_devices)
			{
				if (_Configuration._AudioDevice.Get() == queried_audio_device._Name)
				{
					audio_device = &queried_audio_device;

					break;
				}
			}
		}

		SoundSubSystem::InitializationParameters initialization_parameters;

		initialization_parameters._AudioDevice = audio_device;

		_SubSystem->Initialize(initialization_parameters);
	}

	//Initialize the platform.
	PlatformInitialize(_Configuration);
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{
	//Wait for the mixing thread to finish.
	_MixingThread.Join();

	//Terminate the sub system.
	_SubSystem->Terminate();

	//Destroy the sub system.
	delete _SubSystem;

	//Terminate the platform.
	PlatformTerminate();
}

/*
*	Sets the sound sub system type.
*/
void SoundSystem::SetSoundSubSystemType(const SoundSubSystemType sound_sub_system_type) NOEXCEPT
{
	//Stop mixing.
	_ShouldMix.Clear();

	//Wait for the mixing thread to stop mixing.
	while (_IsMixing.IsSet())
	{
		Concurrency::CurrentThread::Pause();
	}

	//Destroy the current sound system, if there is one.
	if (_SubSystem)
	{
		//Terminate the sub system.
		_SubSystem->Terminate();

		//Destroy the sub system.
		delete _SubSystem;
	}

	//Create the sub system.
	switch (sound_sub_system_type)
	{
		case SoundSubSystemType::WASAPI:
		case SoundSubSystemType::DEFAULT:
		{
			_SubSystem = new SoundSubSystemWASAPI();

			break;
		}

		case SoundSubSystemType::ASIO:
		{
			_SubSystem = new SoundSubSystemASIO();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the sound system.
	_SubSystem->_SoundSystem = this;

	//Initialize the sub system!
	{
		//If the audio device is set, find and set it!
		DynamicArray<AudioDevice> audio_devices;
		AudioDevice *RESTRICT audio_device{ nullptr };

		if (_Configuration._AudioDevice.Valid())
		{
			SoundSystem::Instance->QueryAudioDevices(&audio_devices);

			for (AudioDevice &queried_audio_device : audio_devices)
			{
				if (_Configuration._AudioDevice.Get() == queried_audio_device._Name)
				{
					audio_device = &queried_audio_device;

					break;
				}
			}
		}

		SoundSubSystem::InitializationParameters initialization_parameters;

		initialization_parameters._AudioDevice = audio_device;

		_SubSystem->Initialize(initialization_parameters);
	}

	//The mixing thread can mix once again. (:
	_ShouldMix.Set();
}

/*
*	Queries for audio devices.
*/
void SoundSystem::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	ASSERT(_SubSystem, "Needs a sub system to query audio devices!");

	_SubSystem->QueryAudioDevices(audio_devices);
}

/*
*	Opens the given audio device.
*/
void SoundSystem::OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT
{
	//Stop mixing.
	_ShouldMix.Clear();

	//Wait for the mixing thread to stop mixing.
	while (_IsMixing.IsSet())
	{
		Concurrency::CurrentThread::Pause();
	}

	//Cache the sound sub system type.
	SoundSubSystemType sound_sub_system_type{ _Configuration._SoundSubSystemType };

	//Destroy the current sound system, if there is one.
	if (_SubSystem)
	{
		//Use the current sub system's type.
		sound_sub_system_type = _SubSystem->_Type;

		//Terminate the sub system.
		_SubSystem->Terminate();

		//Destroy the sub system.
		delete _SubSystem;
	}

	//Create the sub system.
	switch (sound_sub_system_type)
	{
		case SoundSubSystemType::WASAPI:
		case SoundSubSystemType::DEFAULT:
		{
			_SubSystem = new SoundSubSystemWASAPI();

			break;
		}

		case SoundSubSystemType::ASIO:
		{
			_SubSystem = new SoundSubSystemASIO();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the sound system.
	_SubSystem->_SoundSystem = this;

	//Initialize the sub system!
	{
		SoundSubSystem::InitializationParameters initialization_parameters;

		initialization_parameters._AudioDevice = audio_device;

		_SubSystem->Initialize(initialization_parameters);
	}

	//The mixing thread can mix once again. (:
	_ShouldMix.Set();
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	return _SubSystem->GetNumberOfChannels();
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
float32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	return _SubSystem->GetSampleRate();
}

/*
*	Returns the sound format.
*/
SoundFormat SoundSystem::GetSoundFormat() const NOEXCEPT
{
	return _SubSystem->GetSoundFormat();
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
uint32 SoundSystem::GetAudioLatency() const NOEXCEPT
{
	//Construct the final latency.
	float32 final_latency{ 0.0f };

	//Add the sub system latency.
	final_latency += _SubSystem->GetAudioLatency();

	//Add the buffer size delay.
	final_latency += static_cast<float32>(_SubSystem->GetBufferSize()) / GetSampleRate() * 1'000.0f;

	//Lastly, add the time it takes for the sound system to process sounds.
	final_latency += (static_cast<float32>(DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) * static_cast<float32>(DEFAULT_NUMBER_OF_MIXING_BUFFERS)) / GetSampleRate() * 1'000.0f;

	//Return the final latency.
	return BaseMath::Round<uint32>(final_latency);
}

/*
*	Opens an input stream on the given audio device, with the given parameters.
*/
void SoundSystem::OpenInputStream
(
	const uint32 start_channel_index,
	const uint32 number_of_channels,
	InputStreamCallback input_stream_callback,
	void *const RESTRICT user_data
) NOEXCEPT
{
	_SubSystem->OpenInputStream(start_channel_index, number_of_channels, input_stream_callback, user_data);
}

/*
*	Adds a mix component to the master mix channel. Returns the unique identifier for that sound mix component.
*/
uint64 SoundSystem::AddMasterChannelSoundMixComponent(const SoundMixComponent &component) NOEXCEPT
{
	//Queue the master channel sound mix component.
	SoundSystemData::_QueuedAddMasterChannelSoundMixComponents.Push(component);

	//Return the unique identifier for this sound mix component.
	return component._Identifier;
}

/*
*	Removes a sound mix component from the master mix channel.
*/
void SoundSystem::RemoveMasterChannelSoundMixComponent(const uint64 identifier) NOEXCEPT
{
	//Queue the master channel sound mix component.
	SoundSystemData::_QueuedRemoveMasterChannelSoundMixComponents.Push(identifier);
}

/*
*	Plays a sound.
*/
void SoundSystem::PlaySound(const PlaySound2DRequest &request) NOEXCEPT
{
	//Queue the play sound request.
	QueuedPlaySoundRequest queued_play_sound_request;

	queued_play_sound_request._SoundAsset = request._SoundAsset;
	queued_play_sound_request._Gain = request._Gain;
	queued_play_sound_request._Pan = request._Pan;
	queued_play_sound_request._StartTime = request._StartTime;
	queued_play_sound_request._PlaybackRate = request._PlaybackRate;
	queued_play_sound_request._IsLooping = request._IsLooping;
	queued_play_sound_request._AttackTime = request._AttackTime;
	queued_play_sound_request._DecayTime = request._DecayTime;
	queued_play_sound_request._SustainGain = request._SustainGain;
	queued_play_sound_request._ReleaseTime = request._ReleaseTime;
	queued_play_sound_request._SoundInstanceHandle = _SoundInstanceCounter++;
	queued_play_sound_request._SoundStartedCallback = request._SoundStartedCallback;
	queued_play_sound_request._SoundStoppedCallback = request._SoundStoppedCallback;
	queued_play_sound_request._AudioTimeTracker = request._AudioTimeTracker;

	if (request._SoundInstance)
	{
		*request._SoundInstance = queued_play_sound_request._SoundInstanceHandle;
	}

	SoundSystemData::_QueuedPlaySoundRequests.Push(queued_play_sound_request);
}

/*
*	Stops a sound.
*/
void SoundSystem::StopSound(const SoundInstanceHandle handle) NOEXCEPT
{
	//Queue the stop sound request.
	QueuedStopSoundRequest queued_stop_sound_request;

	queued_stop_sound_request._SoundInstanceHandle = handle;

	SoundSystemData::_QueuedStopSoundRequests.Push(queued_stop_sound_request);
}

/*
*	Returns whether or not the sound system is currently muted.
*/
NO_DISCARD bool SoundSystem::IsCurrentlyMuted() const NOEXCEPT
{
	return _IsMuted.IsSet();
}

/*
*	Mutes the sound system.
*/
void SoundSystem::Mute() NOEXCEPT
{
	_IsMuted.Set();
}

/*
*	Un-mutes the sound system.
*/
void SoundSystem::UnMute() NOEXCEPT
{
	_IsMuted.Clear();
}

/*
*	Returns whether or not the sound system is currently paused.
*/
NO_DISCARD bool SoundSystem::IsCurrentlyPaused() const NOEXCEPT
{
	return _IsPaused.IsSet();
}

/*
*	Pauses the sound system.
*/
void SoundSystem::Pause() NOEXCEPT
{
	_IsPaused.Set();
}

/*
*	Un-pauses the sound system.
*/
void SoundSystem::UnPause() NOEXCEPT
{
	_IsPaused.Clear();
}

/*
*	Returns if the sound system is currently recording.
*/
NO_DISCARD bool SoundSystem::IsCurrentlyRecording() const NOEXCEPT
{
	return _ShouldRecord.IsSet();
}

/*
*	Starts recording.
*	Can report the expected length, in seconds, to give the sound system a heads up about how much memory needs to be allocated.
*/
void SoundSystem::StartRecording(SoundAsset *const RESTRICT recording_sound_resource, const float32 expected_length) NOEXCEPT
{
	//Set the recording sound asset.
	_RecordingSoundAsset = recording_sound_resource;

	//Reset the recording sound asset.
	_RecordingSoundAsset->_Samples.Clear();

	//Set up the recording sound asset.
	_RecordingSoundAsset->_SampleRate = GetSampleRate();
	_RecordingSoundAsset->_NumberOfChannels = GetNumberOfChannels();
	_RecordingSoundAsset->_Samples.Upsize<true>(2);

	//Allocate the required amount of data, if the expected length is reported.
	if (expected_length > 0.0f)
	{
		_RecordingSoundAsset->_Samples[0].Reserve(BaseMath::Ceiling<uint64>(expected_length * GetSampleRate()));
		_RecordingSoundAsset->_Samples[1].Reserve(BaseMath::Ceiling<uint64>(expected_length * GetSampleRate()));
	}

	//Set the flag.
	_ShouldRecord.Set();
}

/*
*	Stops recording. Saves the recording to a .WAV file to the specified file path.
*/
void SoundSystem::StopRecording() NOEXCEPT
{
	//Clear the flag.
	_ShouldRecord.Clear();

	//Wait for the sound system to stop recording.
	_IsRecording.Wait<WaitMode::YIELD>();
}

/*
*	Initializes the mixing buffers.
*/
void SoundSystem::InitializeMixingBuffers(const uint8 number_of_mixing_buffers, const uint32 number_of_samples_per_mixing_buffer) NOEXCEPT
{
	//If mixing buffers are already created, terminate them.
	if (!_MixingBuffers.Empty())
	{
		TerminateMixingBuffers();
	}

	//Set the number of mixing buffers.
	_NumberOfMixingBuffers = number_of_mixing_buffers;

	//Set the number of samples per mixing buffer.
	_NumberOfSamplesPerMixingBuffer = number_of_samples_per_mixing_buffer;

	//Cache the number of channels.
	const uint8 number_of_channels{ GetNumberOfChannels() };

	//Cache the sound format.
	const SoundFormat sound_format{ GetSoundFormat() };

	//Initialize all mixing buffers.
	_MixingBuffers.Upsize<false>(number_of_mixing_buffers);

	for (uint8 i{ 0 }; i < number_of_mixing_buffers; ++i)
	{
		_MixingBuffers[i] = Memory::Allocate(_NumberOfSamplesPerMixingBuffer * number_of_channels * (GetNumberOfBitsPerSample(sound_format) / 8));
	}

	//Allocate the intermediate mixing buffer.
	_IntermediateMixingBuffer = static_cast<float32 *const RESTRICT>(Memory::Allocate(_NumberOfSamplesPerMixingBuffer * number_of_channels * sizeof(float32)));

	//The mixing buffers are now initialized!
	_MixingBuffersInitialized = true;

	//The sound system can mix now.
	_ShouldMix.Set();
}

/*
*	Terminates the mixing buffers.
*/
void SoundSystem::TerminateMixingBuffers() NOEXCEPT
{
	//The sound system should no longer mix.
	_ShouldMix.Clear();

	//Wait for the sound system to smixing.
	while (_IsMixing.IsSet())
	{
		Concurrency::CurrentThread::Pause();
	}

	//Free the mixing buffers.
	for (void *const RESTRICT mixing_buffer : _MixingBuffers)
	{
		Memory::Free(mixing_buffer);
	}

	_MixingBuffers.Clear();

	//Free the intermediate mixing buffer.
	Memory::Free(_IntermediateMixingBuffer);

	//Reset the current mixing buffer write index.
	_CurrentMixingBufferWriteIndex = 0;

	//Reset the current mixing buffer read index.
	_CurrentMixingBufferReadIndex = 0;

	//Reset the number of mixing buffers ready.
	_MixingBuffersReady.Store(0);

	//Reset the current sample read index.
	_CurrentSampleReadIndex = 0;

	//The mixing buffers are no longer initialized.
	_MixingBuffersInitialized = false;
}

/*
*	Performs mixing.
*/
void SoundSystem::Mix() NOEXCEPT
{
	//Return if the game is shutting down.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		PROFILING_SCOPE("SoundSystem_Mix");

		//Need the platform to have been initialized before the mixing buffers has been initialized. And no mixing is done when paused.
		if (!_ShouldMix.IsSet() || !_SubSystem->IsInitialized() || !_MixingBuffersInitialized || IsCurrentlyPaused() || (_ContinueWhileEnginePaused ? false : CatalystEngineSystem::Instance->IsEnginePaused()))
		{
			//The sound system is not mixing.
			_IsMixing.Clear();

			continue;
		}

		//The sound system is mixing.
		_IsMixing.Set();

		//Cache the number of channels.
		const uint8 number_of_channels{ GetNumberOfChannels() };

		//Cache the sound format.
		const SoundFormat sound_format{ GetSoundFormat() };

		//Remove all queued master channel sound mix components.
		{
			Optional<uint64> identifier{ SoundSystemData::_QueuedRemoveMasterChannelSoundMixComponents.Pop() };

			while (identifier.Valid())
			{
				for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
				{
					for (uint64 component_index{ 0 }, size{ _MasterChannelMixComponents[channel_index].Size() }; component_index < size; ++component_index)
					{
						if (_MasterChannelMixComponents[channel_index][component_index]._Identifier == identifier.Get())
						{
							_MasterChannelMixComponents[channel_index].EraseAt<false>(component_index);

							break;
						}
					}
				}

				identifier = SoundSystemData::_QueuedRemoveMasterChannelSoundMixComponents.Pop();
			}
		}

		//Add all queued master channel sound mix components.
		{
			Optional<SoundMixComponent> component{ SoundSystemData::_QueuedAddMasterChannelSoundMixComponents.Pop() };

			while (component.Valid())
			{
				for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
				{
					_MasterChannelMixComponents[channel_index].Emplace(component.Get());
				}

				component = SoundSystemData::_QueuedAddMasterChannelSoundMixComponents.Pop();
			}
		}

		//Add all queued play sound requests to the playing sounds.
		{
			Optional<QueuedPlaySoundRequest> queued_play_sound_request{ SoundSystemData::_QueuedPlaySoundRequests.Pop() };

			while (queued_play_sound_request.Valid())
			{
				PlayingSound new_playing_sound;

				new_playing_sound._SoundAssetPlayer.SetSoundAsset(queued_play_sound_request.Get()._SoundAsset);
				new_playing_sound._SoundAssetPlayer.SetGain(queued_play_sound_request.Get()._Gain);
				new_playing_sound._SoundAssetPlayer.SetPan(queued_play_sound_request.Get()._Pan);
				new_playing_sound._SoundAssetPlayer.SetPlaybackSpeed(queued_play_sound_request.Get()._SoundAsset->_SampleRate / GetSampleRate() * queued_play_sound_request.Get()._PlaybackRate);
				new_playing_sound._SoundAssetPlayer.SetIsLooping(queued_play_sound_request.Get()._IsLooping);

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).SetSampleRate(GetSampleRate());
					new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).SetStageValues
					(
						queued_play_sound_request.Get()._AttackTime,
						queued_play_sound_request.Get()._DecayTime,
						queued_play_sound_request.Get()._SustainGain,
						queued_play_sound_request.Get()._ReleaseTime
					);
					new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).EnterAttackStage();
				}

				new_playing_sound._SoundAssetPlayer.SetCurrentSample(static_cast<int64>(queued_play_sound_request.Get()._StartTime * queued_play_sound_request.Get()._SoundAsset->_SampleRate));
				new_playing_sound._SoundInstanceHandle = queued_play_sound_request.Get()._SoundInstanceHandle;
				new_playing_sound._SoundStoppedCallback = queued_play_sound_request.Get()._SoundStoppedCallback;
				new_playing_sound._AudioTimeTracker = queued_play_sound_request.Get()._AudioTimeTracker;

				if (new_playing_sound._AudioTimeTracker)
				{
					new_playing_sound._AudioTimeTracker->Store(static_cast<float64>(queued_play_sound_request.Get()._StartTime));
				}

				SoundSystemData::_PlayingSounds.Emplace(new_playing_sound);

				ASSERT(SoundSystemData::_PlayingSounds.Capacity() == SoundSystemConstants::MAXIMUM_NUMBER_OF_PLAYING_SOUNDS, "Growing dynamic array in SoundSystem::Mix(), this is bad!");

				if (queued_play_sound_request.Get()._SoundStartedCallback)
				{
					queued_play_sound_request.Get()._SoundStartedCallback(queued_play_sound_request.Get()._SoundInstanceHandle);
				}

				queued_play_sound_request = SoundSystemData::_QueuedPlaySoundRequests.Pop();
			}
		}

		//Stop all queued stop sound requests.
		{
			Optional<QueuedStopSoundRequest> queued_stop_sound_request{ SoundSystemData::_QueuedStopSoundRequests.Pop() };

			while (queued_stop_sound_request.Valid())
			{
				for (uint64 i{ 0 }, size{ SoundSystemData::_PlayingSounds.Size() }; i < size; ++i)
				{
					if (SoundSystemData::_PlayingSounds[i]._SoundInstanceHandle == queued_stop_sound_request.Get()._SoundInstanceHandle)
					{
						SoundSystemData::_PlayingSounds[i]._SoundAssetPlayer.Stop();

						break;
					}
				}

				queued_stop_sound_request = SoundSystemData::_QueuedStopSoundRequests.Pop();
			}
		}

		//Make mixing buffers ready. (:
		if (_MixingBuffersReady.Load() < _NumberOfMixingBuffers)
		{
			//Reset the intermediate mixing buffer.
			Memory::Set(_IntermediateMixingBuffer, 0, _NumberOfSamplesPerMixingBuffer * number_of_channels * sizeof(float32));

			//Fetch all the samples from the playing sounds.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (PlayingSound &playing_sound : SoundSystemData::_PlayingSounds)
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer; ++sample_index)
					{
						_IntermediateMixingBuffer[sample_index * number_of_channels + channel_index] += playing_sound._SoundAssetPlayer.NextSample(channel_index);
						playing_sound._SoundAssetPlayer.Advance(channel_index);
					}

					//Update the audio time tracker, if it exists.
					if (playing_sound._AudioTimeTracker)
					{
						playing_sound._AudioTimeTracker->Store(playing_sound._SoundAssetPlayer.GetCurrentAudioTime());
					}
				}
			}

			//Process all the samples.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (SoundMixComponent &component : _MasterChannelMixComponents[channel_index])
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer; ++sample_index)
					{
						component.Process(&_IntermediateMixingBuffer[sample_index * number_of_channels + channel_index]);
					}
				}
			}

			switch (sound_format)
			{
				case SoundFormat::SIGNED_INTEGER_8_BIT:
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer * number_of_channels; ++sample_index)
					{
						static_cast<int8 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[sample_index] = static_cast<int8>(_IntermediateMixingBuffer[sample_index] * static_cast<float32>(INT8_MAXIMUM));
					}

					break;
				}

				case SoundFormat::SIGNED_INTEGER_16_BIT:
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer * number_of_channels; ++sample_index)
					{
						static_cast<int16 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[sample_index] = static_cast<int16>(_IntermediateMixingBuffer[sample_index] * static_cast<float32>(INT16_MAXIMUM));
					}

					break;
				}

				case SoundFormat::SIGNED_INTEGER_24_BIT:
				{
					//Unsure about this, find an audio device that supports it and test it. :x
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer * number_of_channels; ++sample_index)
					{
						const byte *const RESTRICT destination{ static_cast<byte *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex]) + (sample_index * 3) };
						const int32 value{ static_cast<int32>(_IntermediateMixingBuffer[sample_index] * static_cast<float32>(INT24_MAXIMUM)) };

						Memory::Copy(destination, &value, 3);
					}

					break;
				}

				case SoundFormat::SIGNED_INTEGER_32_BIT:
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer * number_of_channels; ++sample_index)
					{
						static_cast<int32 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[sample_index] = static_cast<int32>(_IntermediateMixingBuffer[sample_index] * static_cast<float32>(INT32_MAXIMUM));
					}

					break;
				}

				case SoundFormat::FLOAT_32_BIT:
				{
					Memory::Copy(_MixingBuffers[_CurrentMixingBufferWriteIndex], _IntermediateMixingBuffer, _NumberOfSamplesPerMixingBuffer* number_of_channels * sizeof(float32));

					break;
				}

				case SoundFormat::FLOAT_64_BIT:
				{
					for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer * number_of_channels; ++sample_index)
					{
						static_cast<float64 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[sample_index] = static_cast<float64>(_IntermediateMixingBuffer[sample_index]);
					}

					break;
				}

				default:
				{
					ASSERT(false, "Unhandled case!");

					break;
				}
			}

			//Check if the sound system should record.
			const bool should_record{ _ShouldRecord.IsSet() };

			//Set whether or not the sound is recording now, and if so, copy over the mixed buffer.
			if (should_record)
			{
				_IsRecording.Set();

				uint32 current_sample_index{ 0 };

				for (uint32 sample_index{ 0 }; sample_index < _NumberOfSamplesPerMixingBuffer; ++sample_index)
				{
					for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
					{
						//Write the current value into the recording sound asset.
						switch (sound_format)
						{
							case SoundFormat::SIGNED_INTEGER_8_BIT:
							{
								ASSERT(false, "Implement this plz!");

								break;
							}

							case SoundFormat::SIGNED_INTEGER_16_BIT:
							{
								_RecordingSoundAsset->_Samples[channel_index].Emplace(static_cast<int16 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[current_sample_index++]);

								break;
							}

							case SoundFormat::SIGNED_INTEGER_32_BIT:
							{
								ASSERT(false, "Implement this plz!");

								break;
							}

							case SoundFormat::FLOAT_32_BIT:
							{
								ASSERT(false, "Implement this plz!");

								break;
							}

							default:
							{
								ASSERT(false, "Unhandled case!");

								break;
							}
						}
					}
				}
			}

			else
			{
				_IsRecording.Clear();
			}

			//Update the current mixing buffer write index.
			_CurrentMixingBufferWriteIndex = (_CurrentMixingBufferWriteIndex + 1) % _NumberOfMixingBuffers;

			//A new mixing buffer is ready.
			_MixingBuffersReady.FetchAdd(1);
		}

		//Remove any inactive sounds.
		for (uint64 i{ 0 }; i < SoundSystemData::_PlayingSounds.Size();)
		{
			if (!SoundSystemData::_PlayingSounds[i]._SoundAssetPlayer.IsActive())
			{
				if (SoundSystemData::_PlayingSounds[i]._SoundStoppedCallback)
				{
					SoundSystemData::_PlayingSounds[i]._SoundStoppedCallback(SoundSystemData::_PlayingSounds[i]._SoundInstanceHandle);
				}

				SoundSystemData::_PlayingSounds.EraseAt<false>(i);
			}

			else
			{
				++i;
			}
		}

		while (_MixingBuffersReady.Load() == _NumberOfMixingBuffers)
		{
			Concurrency::CurrentThread::Yield();
		}
	}
}

/*
*	The sound callback.
*/
void SoundSystem::SoundCallback(const float32 sample_rate,
								const SoundFormat sound_format,
								const uint8 number_of_channels,
								const uint32 number_of_samples,
								void *const RESTRICT buffer_data) NOEXCEPT
{
	//If the sound system is currently muted or paused, just fill the buffer with zeroes.
	if (!_SubSystem->IsInitialized() || !_MixingBuffersInitialized || IsCurrentlyMuted() || IsCurrentlyPaused() || (_ContinueWhileEnginePaused ? false : CatalystEngineSystem::Instance->IsEnginePaused()))
	{
		Memory::Set(buffer_data, 0, (GetNumberOfBitsPerSample(sound_format) / 8) * number_of_channels * number_of_samples);
	}

	else
	{
		//Read all samples.
		{	
			//Cache local values.
			uint8 local_mixing_buffer_read_index{ _CurrentMixingBufferReadIndex };
			int32 local_mixing_buffers_ready{ _MixingBuffersReady.Load() };
			uint32 local_sample_read_index{ _CurrentSampleReadIndex };

            //Read all samples.
			uint32 samples_read{ 0 };
			uint32 samples_left_to_read{ number_of_samples };

			while (samples_left_to_read > 0)
			{
				//Calculate the number of samples to read.
				const uint32 number_of_samples_to_read{ BaseMath::Minimum<uint32>(_NumberOfSamplesPerMixingBuffer - local_sample_read_index, samples_left_to_read) };

                if (local_mixing_buffers_ready > 0)
				{
					switch (sound_format)
					{
						case SoundFormat::SIGNED_INTEGER_8_BIT:
						{
							void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 1 * number_of_channels };
							const void *const RESTRICT source{ &static_cast<uint8* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels] };
							const uint64 bytes_to_read{ number_of_samples_to_read * 1 * number_of_channels };

							Memory::Copy(destination, source, bytes_to_read);

							break;
						}

						case SoundFormat::SIGNED_INTEGER_16_BIT:
						{
							void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 2 * number_of_channels };
							const void *const RESTRICT source{ &static_cast<int16* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels]  };
							const uint64 bytes_to_read{ number_of_samples_to_read * 2 * number_of_channels };

							Memory::Copy(destination, source, bytes_to_read);

							break;
						}

						case SoundFormat::SIGNED_INTEGER_32_BIT:
						{
							void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 4 * number_of_channels };
							const void *const RESTRICT source{ &static_cast<int32* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels]  };
							const uint64 bytes_to_read{ number_of_samples_to_read * 4 * number_of_channels };

							Memory::Copy(destination, source, bytes_to_read);

							break;
						}

						case SoundFormat::FLOAT_32_BIT:
						{
							void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 4 * number_of_channels };

							Memory::Copy(destination, &static_cast<float32 *const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels], number_of_samples_to_read * sizeof(float32) * number_of_channels);

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}

                else
                {
                    switch (sound_format)
                    {
						case SoundFormat::SIGNED_INTEGER_8_BIT:
                        {
                            void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 1 * number_of_channels };
                            const uint64 bytes_to_read{ number_of_samples_to_read * 1 * number_of_channels };

                            Memory::Set(destination, 0, bytes_to_read);

                            break;
                        }

						case SoundFormat::SIGNED_INTEGER_16_BIT:
                        {
                            void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 2 * number_of_channels };
                            const uint64 bytes_to_read{ number_of_samples_to_read * 2 * number_of_channels };

                            Memory::Set(destination, 0, bytes_to_read);

                            break;
                        }

						case SoundFormat::SIGNED_INTEGER_32_BIT:
						case SoundFormat::FLOAT_32_BIT:
                        {
                            void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 4 * number_of_channels };
                            const uint64 bytes_to_read{ number_of_samples_to_read * 4 * number_of_channels };

                            Memory::Set(destination, 0, bytes_to_read);

                            break;
                        }

                        default:
                        {
                            ASSERT(false, "Invalid case!");

                            break;
                        }
                    }
                }

				samples_read += number_of_samples_to_read;
				samples_left_to_read -= number_of_samples_to_read;

				local_sample_read_index += number_of_samples_to_read;

				while (local_sample_read_index >= _NumberOfSamplesPerMixingBuffer)
				{
					local_mixing_buffer_read_index = (local_mixing_buffer_read_index + 1) & (_NumberOfMixingBuffers - 1);
					--local_mixing_buffers_ready;
					local_sample_read_index -= _NumberOfSamplesPerMixingBuffer;
				}
			}
		}

		//Update values.
		_CurrentSampleReadIndex += number_of_samples;

		while (_CurrentSampleReadIndex >= _NumberOfSamplesPerMixingBuffer)
		{
			_CurrentMixingBufferReadIndex = (_CurrentMixingBufferReadIndex + 1) & (_NumberOfMixingBuffers - 1);
			_MixingBuffersReady.FetchSub(1);
			_CurrentSampleReadIndex -= _NumberOfSamplesPerMixingBuffer;
		}
	}
}