#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//File.
#include <File/Core/BinaryInputFile.h>

//Third party.
#include <ThirdParty/OpenMP3/openmp3.h>

class MP3Reader final
{

public:

	/*
	*	Reads the sound asset at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, AudioStream *const RESTRICT audio_stream) NOEXCEPT
	{
		//Read the file.
		BinaryInputFile file{ file_path };

		//Read the raw data.
		byte *const RESTRICT raw_data{ static_cast<byte *const RESTRICT>(Memory::Allocate(file.Size())) };
		file.Read(raw_data, file.Size());

		//Create the library.
		OpenMP3::Library library;

		//Create the iterator.
		OpenMP3::Iterator iterator{ library, raw_data, static_cast<OpenMP3::UInt>(file.Size()) };

		//Create the decoder.
		OpenMP3::Decoder decoder{ library };

		//Set the number of channels.
		audio_stream->SetNumberOfChannels(2);

		//Allocate the sample buffers.
		DynamicArray<float32> samples;

		//Process all frames.
		{
			OpenMP3::Frame frame;
			float32 output[2][1152];

			for (;;)
			{
				if (iterator.GetNext(frame))
				{
					const OpenMP3::UInt processed_samples{ decoder.ProcessFrame(frame, output) };

					if (processed_samples > 0)
					{
						audio_stream->SetSampleRate(frame.GetSampleRate());

						for (OpenMP3::UInt sample_index{ 0 }; sample_index < processed_samples; ++sample_index)
						{
							for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
							{
								samples.Emplace(output[channel_index][sample_index]);
							}
						}
					}
				}
				
				else
				{
					break;
				}
			}
		}

		//Close the file.
		file.Close();

		//Free the raw data.
		Memory::Free(raw_data);

		//Set the format.
		audio_stream->SetFormat(Audio::Format::FLOAT_32_BIT);

		//Set the number of samples.
		audio_stream->SetNumberOfSamples(samples.Size() / 2);

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
		//???
		return 0.0;
	}

};