#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//File.
#include <File/Core/BinaryFile.h>

//Resources.
#include <Resources/Reading/SoundResource.h>

class SoundResourceReader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char* const RESTRICT file_path, SoundResource* const RESTRICT resource) NOEXCEPT
	{
		ASSERT(resource, "SoundResourceReader::Read - Invalid resource passed!");

		//Open the file.
		BinaryFile<IOMode::In> file{ file_path };

		if (!file)
		{
			ASSERT(false, "Couldn't read file!");

			return false;
		}

		//Read the header.
		WAVHeader header;
		file.Read(&header._ChunkID, sizeof(char) * 4);
		file.Read(&header._ChunkSize, sizeof(uint32));
		file.Read(&header._Format, sizeof(char) * 4);
		file.Read(&header._SubChunk1ID, sizeof(char) * 4);
		file.Read(&header._SubChunk1Size, sizeof(uint32));
		file.Read(&header._AudioFormat, sizeof(uint16));
		file.Read(&header._NumberOfChannels, sizeof(uint16));
		file.Read(&header._SampleRate, sizeof(uint32));
		file.Read(&header._ByteRate, sizeof(uint32));
		file.Read(&header._BlockAlign, sizeof(uint16));
		file.Read(&header._BitsPerSample, sizeof(uint16));
		file.Read(&header._SubChunk2ID, sizeof(char) * 4);
		file.Read(&header._SubChunk2Size, sizeof(uint32));

		//It's possible we didn't reach the data chunk, just skip if that's the case.
		while (!StringUtilities::IsEqual(header._SubChunk2ID, "data", 4))
		{
			file.Skip(header._SubChunk2Size);

			file.Read(&header._SubChunk2ID, sizeof(char) * 4);
			file.Read(&header._SubChunk2Size, sizeof(uint32));
		}

		//Set the sample rate.
		resource->_SampleRate = static_cast<float>(header._SampleRate);

		//Set the number of channels.
		resource->_NumberOfChannels = static_cast<uint8>(header._NumberOfChannels);

		//Calculate the number of samples.
		const uint16 bytes_per_sample{ static_cast<uint16>(header._BitsPerSample / 8) };
		const uint64 number_of_samples{ header._SubChunk2Size / bytes_per_sample };

		//Read all of the samples.
		resource->_Samples.Reserve(number_of_samples);

		switch (header._BitsPerSample)
		{
			case 16:
			{
				ASSERT(false, "This is not implemented yet!");

				break;
			}

			case 24:
			{
				uint8 temporary[3]{ 0 };

				for (uint64 i{ 0 }; i < number_of_samples; ++i)
				{
					file.Read(&temporary, sizeof(uint8) * 3);

					int32 sample{ (temporary[2] << 16) | (temporary[1] << 8) | temporary[0] };

					if (sample & 0x800000)
					{
						sample = sample | ~0xFFFFFF;
					}

					resource->_Samples.Emplace(static_cast<float>(sample) / static_cast<float>(INT24_MAXIMUM));
				}

				break;
			}

			case 32:
			{
				ASSERT(false, "This is not implemented yet!");

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		ASSERT(file.HasReachedEndOfFile(), "There are still data left to read!");

		//Close the file.
		file.Close();

		return true;
	}

private:

	//WAV header definition.
	class WAVHeader final
	{

	public:

		//The chunk ID.
		char _ChunkID[4];

		//The chunk size.
		uint32 _ChunkSize;

		//The format.
		char _Format[4];

		//The sub chunk 1 ID.
		char _SubChunk1ID[4];

		//The sub chunk 1 size.
		uint32 _SubChunk1Size;

		//The audio format. PCM == 1, other values indicate some form of compression.
		uint16 _AudioFormat;

		//The number of channels. 1 == Mono, 2 == Stereo etc.
		uint16 _NumberOfChannels;

		//The sample rate.
		uint32 _SampleRate;

		//The byte rate. Should equal _SampleRate * _NumberOfChannels * _BitsPerSample / 8.
		uint32 _ByteRate;

		//The block align. Should equal _NumberOfChannels * _BitsPerSample / 8.
		uint16 _BlockAlign;

		//The bits per sample.
		uint16 _BitsPerSample;

		//The sub chunk 2 ID.
		char _SubChunk2ID[4];

		//The sub chunk 2 size.
		uint32 _SubChunk2Size;

	};
};