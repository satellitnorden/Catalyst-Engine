//Header file.
#include <Systems/SoundSystem.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Define the singleton.
DEFINE_SINGLETON(SoundSystem);

/*
*	Queued play sound class definition.
*/
class QueuedPlaySound final
{

public:

	//The sound resource.
	ResourcePointer<SoundResource> _SoundResource;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

};

/*
*	Queued stop sound class definition.
*/
class QueuedStopSound final
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
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS{ 64 };

	//The maximum number of queued stop sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS{ 64 };

}

//Sound system data.
namespace SoundSystemData
{

	//The queued master channel mix components.
	AtomicQueue<SoundMixComponent, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedMasterChannelMixComponents;

	//The queued play sounds.
	AtomicQueue<QueuedPlaySound, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedPlaySounds;

	//The queued stop sounds.
	AtomicQueue<QueuedStopSound, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedStopSounds;

	//The playing sounds.
	DynamicArray<PlayingSound> _PlayingSounds;

}

/*
*	Adds a mix component to the master mix channel.
*/
void SoundSystem::AddMasterChannelMixComponent(const SoundMixComponent &component) NOEXCEPT
{
	//Queue the master channel mix component.
	SoundSystemData::_QueuedMasterChannelMixComponents.Push(component);
}

/*
*	Plays a sound.
*/
void SoundSystem::PlaySound(const ResourcePointer<SoundResource> resource, SoundInstanceHandle *const RESTRICT handle) NOEXCEPT
{
	//Queue the play sound.
	QueuedPlaySound queued_Play_sound;

	queued_Play_sound._SoundResource = resource;
	queued_Play_sound._SoundInstanceHandle = _SoundInstanceCounter++;

	if (handle)
	{
		*handle = queued_Play_sound._SoundInstanceHandle;
	}

	SoundSystemData::_QueuedPlaySounds.Push(queued_Play_sound);
}

/*
*	Stops a sound.
*/
void SoundSystem::StopSound(const SoundInstanceHandle handle) NOEXCEPT
{
	//Queue the stop sound.
	QueuedStopSound queued_stop_sound;

	queued_stop_sound._SoundInstanceHandle = handle;

	SoundSystemData::_QueuedStopSounds.Push(queued_stop_sound);
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
	//CATALYST_BENCHMARK_AVERAGE_SECTION_START();

	//Add all queued master channel mix components.
	while (SoundMixComponent *const RESTRICT component{ SoundSystemData::_QueuedMasterChannelMixComponents.Pop() })
	{
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			_MasterChannelMixComponents[i].Emplace(*component);
		}
	}

	//Add all queued play sounds to the playing sounds.
	while (QueuedPlaySound *const RESTRICT queued_play_sound{ SoundSystemData::_QueuedPlaySounds.Pop() })
	{
		PlayingSound new_playing_sound;

		new_playing_sound._SoundResourcePlayer.SetSoundResource(queued_play_sound->_SoundResource);
		new_playing_sound._SoundResourcePlayer.SetPlaybackSpeed(queued_play_sound->_SoundResource->_SampleRate / GetSampleRate());
		new_playing_sound._SoundInstanceHandle = queued_play_sound->_SoundInstanceHandle;

		SoundSystemData::_PlayingSounds.Emplace(new_playing_sound);
	}

	//Stop all queued stop sounds.
	while (QueuedStopSound *const RESTRICT queued_stop_sound{ SoundSystemData::_QueuedStopSounds.Pop() })
	{
		for (uint64 i{ 0 }, size{ SoundSystemData::_PlayingSounds.Size() }; i < size; ++i)
		{
			if (SoundSystemData::_PlayingSounds[i]._SoundInstanceHandle == queued_stop_sound->_SoundInstanceHandle)
			{
				SoundSystemData::_PlayingSounds.EraseAt(i);

				break;
			}
		}
	}

	//Write all samples.
	uint64 sample_index{ 0 };

	for (uint32 i{ 0 }; i < number_of_samples; ++i)
	{
		for (uint8 j{ 0 }; j < number_of_channels; ++j)
		{
			//Calculate the current sample.
			float32 current_sample{ 0.0f };

			for (PlayingSound &playing_sound : SoundSystemData::_PlayingSounds)
			{
				current_sample += static_cast<float32>(playing_sound._SoundResourcePlayer.NextSample(j)) / static_cast<float32>(INT16_MAXIMUM);
			}

			//Apply the master channel mix components.
			for (SoundMixComponent &component : _MasterChannelMixComponents[j])
			{
				component.Process(&current_sample);
			}

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
		for (PlayingSound &playing_sound : SoundSystemData::_PlayingSounds)
		{
			playing_sound._SoundResourcePlayer.Advance();
		}
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

	//CATALYST_BENCHMARK_AVERAGE_SECTION_END("SoundSystem::SoundCallback");
}