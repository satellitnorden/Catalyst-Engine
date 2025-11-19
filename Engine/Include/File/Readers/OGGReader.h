#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//Third party.
#include <ThirdParty/minivorbis.h>

class OGGReader final
{

public:

	/*
	*	Reads the sound asset at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, AudioStream *const RESTRICT audio_stream) NOEXCEPT
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
		audio_stream->SetSampleRate(static_cast<uint32>(info->rate));

		//Set the number of channels.
		audio_stream->SetNumberOfChannels(static_cast<uint8>(info->channels));

		//Set up the samples.
		DynamicArray<float32> samples;

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
				for (long sample_index{ 0 }; sample_index < number_of_samples_read; ++sample_index)
				{
					for (int channel_index{ 0 }; channel_index < info->channels; ++channel_index)
					{
						samples.Emplace(buffer[channel_index][sample_index]);
					}
				}
			}
		}

		//Close the file.
		fclose(sound_file);

		//Set the format.
		audio_stream->SetFormat(Audio::Format::FLOAT_32_BIT);

		//Set the number of samples.
		audio_stream->SetNumberOfSamples(samples.Size() / info->channels);

		//Set the data.
		audio_stream->SetDataInternal(reinterpret_cast<const byte *const RESTRICT>(samples.Data()));

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
		return static_cast<float64>(total_samples_read) / static_cast<float64>(info->rate);
	}

};