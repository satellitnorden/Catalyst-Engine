#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File handling.
#include <FileHandling/BinaryFile.h>

//Resources.
#include <Resources/Reading/SoundResource.h>

class SoundResourceReader final
{

public:

	/*
	*	Reads the sound resource at the given file path.
	*/
	FORCE_INLINE static void Read(const char* const RESTRICT file_path, SoundResource* const RESTRICT resource) NOEXCEPT
	{
		ASSERT(resource, "SoundResourceReader::Read - Invalid resource passed!");

		//Open the file.
		BinaryFile<IOMode::In> file{ file_path };

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
		while (strcmp(header._SubChunk2ID, "data") != 0)
		{
			file.Skip(header._SubChunk2Size);

			file.Read(&header._SubChunk2ID, sizeof(char) * 4);
			file.Read(&header._SubChunk2Size, sizeof(uint32));
		}

		//Set the sample rate.
		resource->_SampleRate = static_cast<float>(header._SampleRate);

		//Calculate the number of samples.
		const uint64 number_of_samples{ header._SubChunk2Size / header._BitsPerSample / 8 };

		//Read all of the samples.
		resource->_Samples.Reserve(number_of_samples);

		for (uint64 i{ 0 }; i < number_of_samples; ++i)
		{

		}

		//Close the file.
		file.Close();
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