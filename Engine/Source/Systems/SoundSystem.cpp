//Header file.
#include <Systems/SoundSystem.h>

//File.
#include <File/Writers/WAVWriter.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Define the singleton.
DEFINE_SINGLETON(SoundSystem);

/*
*	Queued play sound request class definition.
*/
class QueuedPlaySoundRequest final
{

public:

	//The sound resource.
	ResourcePointer<SoundResource> _SoundResource;

	//The gain.
	float32 _Gain;

	//The pan.
	float32 _Pan;

	//The start time.
	float32 _StartTime;

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

	//The sound resource players.
	SoundResourcePlayer _SoundResourcePlayer;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

};

//Sound system constants.
namespace SoundSystemConstants
{

	//The maximum number of queued master channel mix components.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS{ 2 };

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

	//Initialize the platform.
	PlatformInitialize();
}

/*
*	Terminates the sound system.
*/
void SoundSystem::Terminate() NOEXCEPT
{
	//Wait for the mixing thread to finish.
	_MixingThread.Join();

	//Terminate the platform.
	PlatformTerminate();
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
void SoundSystem::PlaySound(const PlaySoundRequest &request) NOEXCEPT
{
	//Queue the play sound request.
	QueuedPlaySoundRequest queued_play_sound_request;

	queued_play_sound_request._SoundResource = request._SoundResource;
	queued_play_sound_request._Gain = request._Gain;
	queued_play_sound_request._Pan = request._Pan;
	queued_play_sound_request._StartTime = request._StartTime;
	queued_play_sound_request._IsLooping = request._IsLooping;
	queued_play_sound_request._AttackTime = request._AttackTime;
	queued_play_sound_request._DecayTime = request._DecayTime;
	queued_play_sound_request._SustainGain = request._SustainGain;
	queued_play_sound_request._ReleaseTime = request._ReleaseTime;
	queued_play_sound_request._SoundInstanceHandle = _SoundInstanceCounter++;

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
void SoundSystem::StartRecording(const float32 expected_length) NOEXCEPT
{
	//Reset the recording sound resource.
	_RecordingSoundResource._Samples.Clear();

	//Set up the recording sound resource.
	_RecordingSoundResource._SampleRate = GetSampleRate();
	_RecordingSoundResource._NumberOfChannels = GetNumberOfChannels();
	_RecordingSoundResource._Samples.Upsize<true>(2);

	//Allocate the required amount of data, if the expected length is reported.
	if (expected_length > 0.0f)
	{
		_RecordingSoundResource._Samples[0].Reserve(CatalystBaseMath::Ceiling<uint64>(expected_length * GetSampleRate()));
		_RecordingSoundResource._Samples[1].Reserve(CatalystBaseMath::Ceiling<uint64>(expected_length * GetSampleRate()));
	}

	//Set the flag.
	_ShouldRecord.Set();
}

/*
*	Stops recording. Saves the recording to a .WAV file to the specified file path.
*/
void SoundSystem::StopRecording(const char *const RESTRICT file_path) NOEXCEPT
{
	//Clear the flag.
	_ShouldRecord.Clear();

	//Wait for the sound system to stop recording.
	_IsRecording.Wait<WaitMode::YIELD>();

	//Write the recording to the given file path.
	WAVWriter::Write(file_path, _RecordingSoundResource);
}

#include <Systems/InputSystem.h>

/*
*	Performs mixing.
*/
void SoundSystem::Mix() NOEXCEPT
{
	//Return if the game is shutting down.
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Need the platform to have been initialized before the mixing buffers has been initialized.
		if (!PlatformInitialized())
		{
			continue;
		}

		//Cache the number of channels.
		const uint8 number_of_channels{ GetNumberOfChannels() };

		//Cache the number of bits per sample.
		const uint8 number_of_bits_per_sample{ GetNumberOfBitsPerSample() };

		//Initialize the mixing buffers, if necessary.
		if (!_MixingBuffersInitialized)
		{
			//Initialize all mixing buffers.
			for (uint8 i{ 0 }; i < NUMBER_OF_MIXING_BUFFERS; ++i)
			{
				_MixingBuffers[i] = Memory::Allocate(NUMBER_OF_SAMPLES_PER_MIXING_BUFFER * (number_of_bits_per_sample / 8) * number_of_channels);
			}

			//The mixing buffers are now initialized!
			_MixingBuffersInitialized = true;
		}

		//Remove all queued master channel sound mix components.
		while (uint64 *const RESTRICT identifier{ SoundSystemData::_QueuedRemoveMasterChannelSoundMixComponents.Pop() })
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				for (uint64 component_index{ 0 }, size{ _MasterChannelMixComponents[i].Size() }; component_index < size; ++component_index)
				{
					if (_MasterChannelMixComponents[i][component_index]._Identifier == *identifier)
					{
						_MasterChannelMixComponents[i].EraseAt(component_index);

						break;
					}
				}
			}
		}

		//Add all queued master channel sound mix components.
		while (SoundMixComponent *const RESTRICT component{ SoundSystemData::_QueuedAddMasterChannelSoundMixComponents.Pop() })
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				_MasterChannelMixComponents[i].Emplace(*component);
			}
		}

		//Add all queued play sound requests to the playing sounds.
		while (QueuedPlaySoundRequest *const RESTRICT queued_play_sound_request{ SoundSystemData::_QueuedPlaySoundRequests.Pop() })
		{
			PlayingSound new_playing_sound;

			new_playing_sound._SoundResourcePlayer.SetSoundResource(queued_play_sound_request->_SoundResource);
			new_playing_sound._SoundResourcePlayer.SetGain(queued_play_sound_request->_Gain);
			new_playing_sound._SoundResourcePlayer.SetPan(queued_play_sound_request->_Pan);
			new_playing_sound._SoundResourcePlayer.SetPlaybackSpeed(queued_play_sound_request->_SoundResource->_SampleRate / GetSampleRate());
			new_playing_sound._SoundResourcePlayer.SetIsLooping(queued_play_sound_request->_IsLooping);
			new_playing_sound._SoundResourcePlayer.GetADSREnvelope().SetSampleRate(GetSampleRate());
			new_playing_sound._SoundResourcePlayer.GetADSREnvelope().SetStageValues(queued_play_sound_request->_AttackTime,
																					queued_play_sound_request->_DecayTime,
																					queued_play_sound_request->_SustainGain,
																					queued_play_sound_request->_ReleaseTime);
			new_playing_sound._SoundResourcePlayer.GetADSREnvelope().EnterAttackStage();
			new_playing_sound._SoundResourcePlayer.SetCurrentSample(static_cast<int64>(queued_play_sound_request->_StartTime * queued_play_sound_request->_SoundResource->_SampleRate));
			new_playing_sound._SoundInstanceHandle = queued_play_sound_request->_SoundInstanceHandle;

			SoundSystemData::_PlayingSounds.Emplace(new_playing_sound);

			ASSERT(SoundSystemData::_PlayingSounds.Capacity() == SoundSystemConstants::MAXIMUM_NUMBER_OF_PLAYING_SOUNDS, "Growing dynamic array in SoundSystem::Mix(), this is bad!");
		}

		//Stop all queued stop sound requests.
		while (QueuedStopSoundRequest *const RESTRICT queued_stop_sound_request{ SoundSystemData::_QueuedStopSoundRequests.Pop() })
		{
			for (uint64 i{ 0 }, size{ SoundSystemData::_PlayingSounds.Size() }; i < size; ++i)
			{
				if (SoundSystemData::_PlayingSounds[i]._SoundInstanceHandle == queued_stop_sound_request->_SoundInstanceHandle)
				{
					SoundSystemData::_PlayingSounds[i]._SoundResourcePlayer.Stop();

					break;
				}
			}
		}

		//Make mixing buffers ready. (:
		if (_MixingBuffersReady < NUMBER_OF_MIXING_BUFFERS)
		{
			//Calculate the conversion multiplier.
			float32 conversion_multiplier;

			switch (number_of_bits_per_sample)
			{
				case 8:
				{
					conversion_multiplier = static_cast<float32>(INT8_MAXIMUM);

					break;
				}

				case 16:
				{
					conversion_multiplier = static_cast<float32>(INT16_MAXIMUM);

					break;
				}

				case 32:
				{
					conversion_multiplier = static_cast<float32>(INT32_MAXIMUM);

					break;
				}

				default:
				{
					ASSERT(false, "Unhandled case!");

					break;
				}
			}

			//Calculate the number of bytes per sample.
			const uint32 number_of_bytes_per_sample{ static_cast<uint32>(number_of_bits_per_sample) >> 3 };

			//Write all samples.
			{
				uint32 current_sample_index{ 0 };

				for (uint32 sample_index{ 0 }; sample_index < NUMBER_OF_SAMPLES_PER_MIXING_BUFFER; ++sample_index)
				{
					for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
					{
						//Calculate the current sample.
						float32 current_sample{ 0.0f };

						//Add all playing sounds to the current sample.
						for (PlayingSound &playing_sound : SoundSystemData::_PlayingSounds)
						{
							current_sample += playing_sound._SoundResourcePlayer.NextSample(channel_index);
						}

						for (SoundMixComponent &component : _MasterChannelMixComponents[channel_index])
						{
							component.Process(&current_sample);
						}

						//Write the current value.
						{
							const int32 converted_sample{ static_cast<int32>(current_sample * conversion_multiplier) };

							void *const RESTRICT conversion_destination{ static_cast<byte* const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex]) + (current_sample_index * number_of_bytes_per_sample) };
							const void *const RESTRICT conversion_source{ &converted_sample };

							Memory::Copy(conversion_destination, conversion_source, number_of_bytes_per_sample);

							++current_sample_index;
						}
					}

					//Advance all playing sounds.
					for (PlayingSound &playing_sound : SoundSystemData::_PlayingSounds)
					{
						playing_sound._SoundResourcePlayer.Advance();
					}
				}
			}

			//Check if the sound system should record.
			const bool should_record{ _ShouldRecord.IsSet() };

			//Set whether or not the sound is recording now, and if so, copy over the mixed buffer.
			if (should_record)
			{
				_IsRecording.Set();

				uint32 current_sample_index{ 0 };

				for (uint32 sample_index{ 0 }; sample_index < NUMBER_OF_SAMPLES_PER_MIXING_BUFFER; ++sample_index)
				{
					for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
					{
						//Write the current value into the recording sound resource.
						switch (number_of_bits_per_sample)
						{
							case 8:
							{
								ASSERT(false, "Inplement this plz!");

								break;
							}

							case 16:
							{
								_RecordingSoundResource._Samples[channel_index].Emplace(static_cast<int16 *const RESTRICT>(_MixingBuffers[_CurrentMixingBufferWriteIndex])[current_sample_index++]);

								break;
							}

							case 32:
							{
								ASSERT(false, "Inplement this plz!");

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
			_CurrentMixingBufferWriteIndex = (_CurrentMixingBufferWriteIndex + 1) & (NUMBER_OF_MIXING_BUFFERS - 1);

			//A new mixing buffer is ready.
			++_MixingBuffersReady;
		}

		//Remove any inactive sounds.
		for (uint64 i{ 0 }; i < SoundSystemData::_PlayingSounds.Size();)
		{
			if (!SoundSystemData::_PlayingSounds[i]._SoundResourcePlayer.IsActive())
			{
				SoundSystemData::_PlayingSounds.EraseAt(i);
			}

			else
			{
				++i;
			}
		}

		//Yield.
		Concurrency::CurrentThread::Yield();
	}
}

/*
*	The sound callback.
*/
void SoundSystem::SoundCallback(const float32 sample_rate,
								const uint8 bits_per_sample,
								const uint8 number_of_channels,
								const uint32 number_of_samples,
								void *const RESTRICT buffer_data) NOEXCEPT
{
	//Read all samples.
	{	
		//Cache local values.
		uint8 local_mixing_buffer_read_index{ _CurrentMixingBufferReadIndex };
		int32 local_mixing_buffers_ready{ _MixingBuffersReady };
		uint32 local_sample_read_index{ _CurrentSampleReadIndex };

		//Read all samples.
		uint32 samples_read{ 0 };
		uint32 samples_left_to_read{ number_of_samples };

		while (samples_left_to_read > 0)
		{
			//Calculate the number of samples to read.
			const uint32 number_of_samples_to_read{ CatalystBaseMath::Minimum<uint32>(NUMBER_OF_SAMPLES_PER_MIXING_BUFFER - local_sample_read_index, samples_left_to_read) };

			if (local_mixing_buffers_ready > 0)
			{
				switch (bits_per_sample)
				{
					case 8:
					{
						void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 1 * number_of_channels };
						const void *const RESTRICT source{ &static_cast<uint8* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels] };
						const uint64 bytes_to_read{ number_of_samples_to_read * 1 * number_of_channels };

						Memory::Copy(destination, source, bytes_to_read);

						break;
					}

					case 16:
					{
						void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 2 * number_of_channels };
						const void *const RESTRICT source{ &static_cast<int16* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels]  };
						const uint64 bytes_to_read{ number_of_samples_to_read * 2 * number_of_channels };

						Memory::Copy(destination, source, bytes_to_read);

						break;
					}

					case 32:
					{
						void *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(buffer_data) + samples_read * 4 * number_of_channels };
						const void *const RESTRICT source{ &static_cast<int32* const RESTRICT>(_MixingBuffers[local_mixing_buffer_read_index])[local_sample_read_index * number_of_channels]  };
						const uint64 bytes_to_read{ number_of_samples_to_read * 4 * number_of_channels };

						Memory::Copy(destination, source, bytes_to_read);

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
				int x = 0;
			}

			samples_read += number_of_samples_to_read;
			samples_left_to_read -= number_of_samples_to_read;

			local_sample_read_index += number_of_samples_to_read;

			while (local_sample_read_index >= NUMBER_OF_SAMPLES_PER_MIXING_BUFFER)
			{
				local_mixing_buffer_read_index = (local_mixing_buffer_read_index + 1) & (NUMBER_OF_MIXING_BUFFERS - 1);
				--local_mixing_buffers_ready;
				local_sample_read_index -= NUMBER_OF_SAMPLES_PER_MIXING_BUFFER;
			}
		}
	}

	//Update values.
	_CurrentSampleReadIndex += number_of_samples;

	while (_CurrentSampleReadIndex >= NUMBER_OF_SAMPLES_PER_MIXING_BUFFER)
	{
		_CurrentMixingBufferReadIndex = (_CurrentMixingBufferReadIndex + 1) & (NUMBER_OF_MIXING_BUFFERS - 1);
		--_MixingBuffersReady;
		_CurrentSampleReadIndex -= NUMBER_OF_SAMPLES_PER_MIXING_BUFFER;
	}
}