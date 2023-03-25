#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Core/BinaryFile.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/OpenMP3/openmp3.h>

class MP3Reader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, SoundResource *const RESTRICT resource) NOEXCEPT
	{
		//Read the file.
		BinaryFile<BinaryFileMode::IN> file{ file_path };

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
		resource->_NumberOfChannels = static_cast<uint8>(2);

		//Allocate the sample buffers.
		resource->_Samples.Upsize<true>(resource->_NumberOfChannels);

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
						resource->_SampleRate = static_cast<float32>(frame.GetSampleRate());

						for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
						{
							for (OpenMP3::UInt sample_index{ 0 }; sample_index < processed_samples; ++sample_index)
							{
								resource->_Samples[channel_index].Emplace(static_cast<int16>(CatalystBaseMath::Clamp(output[channel_index][sample_index], -1.0f, 1.0f) * static_cast<float32>(INT16_MAXIMUM)));
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