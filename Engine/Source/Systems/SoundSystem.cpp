//Header file.
#include <Systems/SoundSystem.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Define the singleton.
DEFINE_SINGLETON(SoundSystem);

/*
*	Queued sound class definition.
*/
class QueuedSound final
{

public:

	//The sound resource.
	ResourcePointer<SoundResource> _SoundResource;

};

/*
*	Playing sound class definition.
*/
class PlayingSound final
{

public:

	//The sound resource players.
	SoundResourcePlayer _SoundResourcePlayer;

};

//Sound system constants.
namespace SoundSystemConstants
{

	//The maximum number of queued master channel mix components.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS{ 2 };

	//The maximum number of queued sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_SOUNDS{ 32 };

}

//Sound system data.
namespace SoundSystemData
{

	//The queued master channel mix components.
	AtomicQueue<SoundMixComponent, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedMasterChannelMixComponents;

	//The queued sounds.
	AtomicQueue<QueuedSound, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedSounds;

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
void SoundSystem::PlaySound(const ResourcePointer<SoundResource> resource) NOEXCEPT
{
	//Queue the sound.
	QueuedSound queued_sound;

	queued_sound._SoundResource = resource;

	SoundSystemData::_QueuedSounds.Push(queued_sound);
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
	//Add all queued master channel mix components.
	while (SoundMixComponent *const RESTRICT component{ SoundSystemData::_QueuedMasterChannelMixComponents.Pop() })
	{
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			_MasterChannelMixComponents[i].Emplace(*component);
		}
	}

	//Add all queued sounds to the playing sounds.
	while (QueuedSound *const RESTRICT queued_sound{ SoundSystemData::_QueuedSounds.Pop() })
	{
		PlayingSound new_playing_sound;

		new_playing_sound._SoundResourcePlayer.SetSoundResource(queued_sound->_SoundResource);

		SoundSystemData::_PlayingSounds.Emplace(new_playing_sound);
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
				current_sample += playing_sound._SoundResourcePlayer.NextSample(j);
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
}