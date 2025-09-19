#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//Content.
#include <Content/Assets/SoundAsset.h>

//File.
#include <File/Core/BinaryInputFile.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>

class WAVReader final
{

public:

	/*
	*	Reads the audio stream at the given file path. Returns if the read was successful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, AudioStream *const RESTRICT audio_stream) NOEXCEPT
	{
		/*
		*	WAV file header class definition.
		*/
		class WAVFileHeader final
		{

		public:

			//The "RIFF" string. Should always equal "RIFF".
			char _RiffString[4];

			//The file size.
			uint32 _FileSize;

			//The "WAVE" string. Should always equal "WAVE".
			char _WaveString[4];

		};

		/*
		*	Chunk class definition.
		*/
		class Chunk final
		{

		public:

			//The identifierr.
			char _Identifier[4];

			//The data size.
			uint32 _DataSize;

		};

		/*
		*	Format chunk class definition.
		*/
		class FormatChunk final
		{

		public:

			//The format type.
			uint16 _FormatType;

			//The number of channels.
			uint16 _NumberOfChannels;

			//The sample rate.
			uint32 _SampleRate;

			//The byters per second. Includes multi-channel data.
			uint32 _BytesPerSecond;

			//The bytes per second. Includes multi-channel data.
			uint16 _BytesPerSample;

			//The bits per sample.
			uint16 _BitsPerSample;

		};

		//Open the file.
		BinaryInputFile file{ file_path };

		//Read the header.
		WAVFileHeader header;
		file.Read(&header, sizeof(WAVFileHeader));

		//Validate the header.
		if (!Memory::Compare(header._RiffString, "RIFF", 4))
		{
			ASSERT(false, "Invalid .wav file!");

			return false;
		}

		if (header._FileSize == 0)
		{
			ASSERT(false, "Invalid .wav file!");

			return false;
		}

		if (!Memory::Compare(header._WaveString, "WAVE", 4))
		{
			ASSERT(false, "Invalid .wav file!");

			return false;
		}

		//Find the "fmt " and "data" chunks and retrieve the data.
		bool found_format_chunk{ false };
		bool found_data_chunk{ false };

		FormatChunk format_chunk;
		DynamicArray<byte> data;

		while (!file.HasReachedEndOfFile() && (!found_format_chunk || !found_data_chunk))
		{
			//Read in the chunk.
			Chunk chunk;
			file.Read(&chunk, sizeof(Chunk));

			if (Memory::Compare(chunk._Identifier, "fmt ", 4))
			{
				file.Read(&format_chunk, sizeof(FormatChunk));

				if (chunk._DataSize > sizeof(FormatChunk))
				{
					file.Skip(chunk._DataSize - sizeof(FormatChunk));
				}

				found_format_chunk = true;
			}

			else if (Memory::Compare(chunk._Identifier, "data", 4))
			{
				data.Upsize<false>(chunk._DataSize);
				file.Read(data.Data(), chunk._DataSize);

				found_data_chunk = true;
			}

			else
			{
				file.Skip(chunk._DataSize);
			}
		}

		if (!found_format_chunk || !found_data_chunk)
		{
			ASSERT(false, "Invalid .wav file!");

			return false;
		}

		//Close the file.
		file.Close();

		//Set the sample rate.
		audio_stream->SetSampleRate(format_chunk._SampleRate);

		//Set the number of channels.
		audio_stream->SetNumberOfChannels(static_cast<uint8>(format_chunk._NumberOfChannels));

		//Set the format.
		switch (format_chunk._FormatType)
		{
			case 1:
			{
				switch (format_chunk._BitsPerSample)
				{
					case 8:
					{
						audio_stream->SetFormat(Audio::Format::INTEGER_8_BIT);

						break;
					}

					case 16:
					{
						audio_stream->SetFormat(Audio::Format::INTEGER_16_BIT);

						break;
					}

					case 24:
					{
						audio_stream->SetFormat(Audio::Format::INTEGER_24_BIT);

						break;
					}

					case 32:
					{
						audio_stream->SetFormat(Audio::Format::INTEGER_32_BIT);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				break;
			}

			case 3:
			{
				switch (format_chunk._BitsPerSample)
				{
					case 32:
					{
						audio_stream->SetFormat(Audio::Format::FLOAT_32_BIT);

						break;
					}

					case 64:
					{
						audio_stream->SetFormat(Audio::Format::FLOAT_64_BIT);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				break;
			}
		
			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the number of samples.
		audio_stream->SetNumberOfSamples(static_cast<uint32>(data.Size() / static_cast<uint64>(format_chunk._NumberOfChannels) / (Audio::BitsPerSample(audio_stream->GetFormat()) / 8)));

		//Set the data.
		audio_stream->SetDataInternal(data.Data());

		//The read was successful. (:
		return true;
	}

	/*
	*	Reads the sound asset at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, SoundAsset *const RESTRICT asset) NOEXCEPT
	{
#if 1
		AudioStream audio_stream;

		if (!Read(file, &audio_stream))
		{
			return false;
		}

		asset->_SampleRate = static_cast<float32>(audio_stream.GetSampleRate());
		asset->_NumberOfChannels = audio_stream.GetNumberOfChannels();

		asset->_Samples.Upsize<true>(asset->_NumberOfChannels);

		for (uint8 channel_index{ 0 }; channel_index < asset->_NumberOfChannels; ++channel_index)
		{
			asset->_Samples[channel_index].Reserve(audio_stream.GetNumberOfSamples());

			for (uint32 sample_index{ 0 }; sample_index < audio_stream.GetNumberOfSamples(); ++sample_index)
			{
				const float32 sample{ audio_stream.Sample(channel_index, sample_index) };

				asset->_Samples[channel_index].Emplace(static_cast<int16>(sample * static_cast<float32>(INT16_MAXIMUM)));
			}
		}

		//The read was successful.
		return true;
#else
		AudioFile<float32> audio_file;

		if (audio_file.load(file))
		{
			asset->_SampleRate = static_cast<float32>(audio_file.getSampleRate());
			asset->_NumberOfChannels = static_cast<uint8>(audio_file.getNumChannels());

			asset->_Samples.Upsize<true>(asset->_NumberOfChannels);

			for (uint64 i{ 0 }, size{ asset->_Samples.Size() }; i < size; ++i)
			{
				asset->_Samples[i].Reserve(audio_file.samples[i].size());

				for (const float32 sample : audio_file.samples[i])
				{
					asset->_Samples[i].Emplace(static_cast<int16>(sample * static_cast<float32>(INT16_MAXIMUM)));
				}
			}

			return true;
		}

		else
		{
			return false;
		}
#endif
	}

	/*
	*	Returns the length of the given audio file.
	*/
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
		BinaryInputFile file{ file_path };

		if (!file)
		{
			ASSERT(false, "Couldn't read file %s!", file_path);

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