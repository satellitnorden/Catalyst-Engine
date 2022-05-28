#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/minivorbis.h>

class OGGReader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, SoundResource *const RESTRICT resource) NOEXCEPT
	{
		//Read the file.
		FILE *const RESTRICT sound_file{ fopen(file, "rb") };

		if (!sound_file)
		{
			return false;
		}

		//Open the sound stream.
		OggVorbis_File vorbis;
		
		if (ov_open_callbacks(sound_file, &vorbis, nullptr, 0, OV_CALLBACKS_DEFAULT) != 0)
		{
			return false;
		}

		//Retrieve the vorbis info.
		vorbis_info *const RESTRICT info{ ov_info(&vorbis, -1) };

		//Set the sample rate.
		resource->_SampleRate = static_cast<float32>(info->rate);

		//Set the number of channels.
		resource->_NumberOfChannels = static_cast<uint8>(info->channels);

		//Set up the channels.
		resource->_Samples.Upsize<true>(resource->_NumberOfChannels);

		//Read the data.
		float **buffer;

		for (;;)
		{
			int32 section{ 0 };
			const long number_of_samples_read{ ov_read_float(&vorbis, &buffer, info->rate, &section) };

			if (number_of_samples_read <= 0)
			{
				break;
			}

			else
			{
				for (uint8 channel_index{ 0 }; channel_index < resource->_NumberOfChannels; ++channel_index)
				{
					for (long sample_index{ 0 }; sample_index < number_of_samples_read; ++sample_index)
					{
						resource->_Samples[channel_index].Emplace(static_cast<int16>(buffer[channel_index][sample_index] * static_cast<float32>(INT16_MAXIMUM)));
					}
				}
			}
		}

		//Return that the read was successful.
		return true;
	}

};