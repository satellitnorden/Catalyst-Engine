#pragma once

#define CATALYST_WAV_READER_IMPLEMENTATION (false)

#if CATALYST_WAV_READER_IMPLEMENTATION

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//File.
#include <File/Core/BinaryFile.h>

//Sound.
#include <Sound/Sound.h>

class WAVReader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, Sound *const RESTRICT resource) NOEXCEPT
	{
		ASSERT(resource, "WAVReader::Read - Invalid resource passed!");

		//Open the file.
		BinaryFile<IOMode::In> input{ file };

		if (!file)
		{
			ASSERT(false, "Couldn't read file!");

			return false;
		}

		//Read the header.
		WAVHeader header;

		input.Read(&header._ChunkID, sizeof(char) * 4);
		input.Read(&header._ChunkSize, sizeof(uint32));
		input.Read(&header._Format, sizeof(char) * 4);
		input.Read(&header._SubChunk1ID, sizeof(char) * 4);
		input.Read(&header._SubChunk1Size, sizeof(uint32));
		input.Read(&header._AudioFormat, sizeof(uint16));
		input.Read(&header._NumberOfChannels, sizeof(uint16));
		input.Read(&header._SampleRate, sizeof(uint32));
		input.Read(&header._ByteRate, sizeof(uint32));
		input.Read(&header._BlockAlign, sizeof(uint16));
		input.Read(&header._BitsPerSample, sizeof(uint16));
		input.Read(&header._SubChunk2ID, sizeof(char) * 4);
		input.Read(&header._SubChunk2Size, sizeof(uint32));

		//It's possible we didn't reach the data chunk, just skip if that's the case.
		while (!StringUtilities::IsEqual(header._SubChunk2ID, "data", 4))
		{
			input.Skip(header._SubChunk2Size);

			input.Read(&header._SubChunk2ID, sizeof(char) * 4);
			input.Read(&header._SubChunk2Size, sizeof(uint32));
		}

		//Set the sample rate.
		resource->_SampleRate = static_cast<float>(header._SampleRate);

		//Set the number of channels.
		resource->_NumberOfChannels = static_cast<uint8>(header._NumberOfChannels);

		//Calculate the number of samples.
		const uint16 bytes_per_sample{ static_cast<uint16>(header._BitsPerSample / 8) };
		const uint64 number_of_samples{ header._SubChunk2Size / bytes_per_sample };

		//Read all of the samples.
		resource->_Channels.Reserve(resource->_NumberOfChannels);

		for (DynamicArray<float32> &channel : resource->_Channels)
		{
			channel.Reserve(number_of_samples / resource->_NumberOfChannels);
		}

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
				uint8 channel_counter{ 0 };

				for (uint64 i{ 0 }; i < number_of_samples; ++i)
				{
					input.Read(&temporary, sizeof(uint8) * 3);

					int32 sample{ (temporary[2] << 16) | (temporary[1] << 8) | temporary[0] };

					if (sample & 0x800000)
					{
						sample = sample | ~0xFFFFFF;
					}

					resource->_Channels[channel_counter].Emplace(static_cast<float32>(sample) / static_cast<float32>(INT24_MAXIMUM));

					//Update the channel counter.
					channel_counter = channel_counter < resource->_Channels.Size() - 1 ? channel_counter + 1 : 0;
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

		ASSERT(input.HasReachedEndOfFile(), "There are still data left to read!");

		//Close the file.
		input.Close();

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

#else

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/AudioFile.h>

class WAVReader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, SoundResource *const RESTRICT resource) NOEXCEPT
	{
		AudioFile<float32> audio_file;

		if (audio_file.load(file))
		{
			resource->_SampleRate = static_cast<float32>(audio_file.getSampleRate());
			resource->_NumberOfChannels = static_cast<uint8>(audio_file.getNumChannels());

			resource->_Samples.Upsize<true>(resource->_NumberOfChannels);

			for (uint64 i{ 0 }, size{ resource->_Samples.Size() }; i < size; ++i)
			{
				resource->_Samples[i].Reserve(audio_file.samples[i].size());

				for (const float32 sample : audio_file.samples[i])
				{
					resource->_Samples[i].Emplace(static_cast<int16>(sample * static_cast<float32>(INT16_MAXIMUM)));
				}
			}

			return true;
		}
		
		else
		{
			return false;
		}
	}

	FORCE_INLINE static NO_DISCARD float64 AudioLength(const char *const RESTRICT file_path) NOEXCEPT
	{
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

		//Open the file.
		BinaryFile<BinaryFileMode::IN> file{ file_path };

		if (!file)
		{
			ASSERT(false, "Couldn't read file" << file_path << "!");

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

		//Close the file.
		file.Close();

		//Return the audio length.
		return static_cast<float64>(header._SubChunk2Size) / (static_cast<float64>(header._NumberOfChannels) * (static_cast<float64>(header._BitsPerSample / 8)));
	}

};

#endif