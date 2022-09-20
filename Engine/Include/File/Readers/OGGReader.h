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

		//Close the file.
		fclose(sound_file);

		//Return that the read was successful.
		return true;
	}

	/*
	*	Returns the length of the given audio file.
	*/
	FORCE_INLINE static NO_DISCARD float64 AudioLength(const char* const RESTRICT file_path) NOEXCEPT
	{
		//Read the file.
		FILE *const RESTRICT sound_file{ fopen(file_path, "rb") };

		if (!sound_file)
		{
			return 0.0;
		}

		//Open the sound stream.
		OggVorbis_File vorbis;

		if (ov_open_callbacks(sound_file, &vorbis, nullptr, 0, OV_CALLBACKS_DEFAULT) != 0)
		{
			return false;
		}

		//Retrieve the vorbis info.
		vorbis_info *const RESTRICT info{ ov_info(&vorbis, -1) };

		//Remember the number of channels.
		const uint8 number_of_channels{ static_cast<uint8>(info->channels) };

		//Read the data.
		float **buffer;
		long total_samples_read{ 0 };

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
				total_samples_read += number_of_samples_read;
			}
		}

		//Close the file.
		fclose(sound_file);

		//Return the audio length.
		return static_cast<float64>(total_samples_read) / static_cast<float64>(info->channels) / static_cast<float64>(info->rate);
	}

};