#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Core/BinaryFile.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>

class WAVWriter final
{

public:

	/*
	*	Writes the sound resource to the given file path. Returns if the write was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Write(const char *const RESTRICT file_path, const SoundResource &resource) NOEXCEPT
	{
#if 1 //Use Catalyst engine implementation.
		//Define constants.
		constexpr uint8 BIT_DEPTH{ 16 };

		//Open the output file.
		BinaryFile<BinaryFileMode::OUT> output_file{ file_path };

		//Add the header chunk.
		{
			constexpr int8 HEADER_CHUNK_1[]
			{
				static_cast<int8>('R'),
				static_cast<int8>('I'),
				static_cast<int8>('F'),
				static_cast<int8>('F')
			};
			output_file.Write(HEADER_CHUNK_1, sizeof(int8) * 4);

			const int32 file_size_in_bytes{ 4 + 24 + 8 + (static_cast<int32>(resource._Samples[0].Size()) * (static_cast<int32>(resource._Samples.Size()) * BIT_DEPTH / 8)) };
			output_file.Write(&file_size_in_bytes, sizeof(int32));

			constexpr int8 HEADER_CHUNK_2[]
			{
				static_cast<int8>('W'),
				static_cast<int8>('A'),
				static_cast<int8>('V'),
				static_cast<int8>('E')
			};
			output_file.Write(HEADER_CHUNK_2, sizeof(int8) * 4);
		}

		//Add the format chunk.
		{
			constexpr int8 FORMAT_CHUNK_1[]
			{
				static_cast<int8>('f'),
				static_cast<int8>('m'),
				static_cast<int8>('t'),
				static_cast<int8>(' ')
			};
			output_file.Write(FORMAT_CHUNK_1, sizeof(int8) * 4);

			const int32 format_chunk_size{ 16 };
			output_file.Write(&format_chunk_size, sizeof(int32));

			const int16 audio_format{ 1 };
			output_file.Write(&audio_format, sizeof(int16));

			const int16 number_of_channels{ static_cast<int16>(resource._Samples.Size()) };
			output_file.Write(&number_of_channels, sizeof(int16));

			const int32 sample_rate{ static_cast<int32>(resource._SampleRate) };
			output_file.Write(&sample_rate, sizeof(int32));

			const int32 number_of_bytes_per_second{ (number_of_channels * sample_rate * BIT_DEPTH) / 8 };
			output_file.Write(&number_of_bytes_per_second, sizeof(int32));

			const int16 number_of_bytes_per_block{ static_cast<int16>(number_of_channels * (BIT_DEPTH / 8)) };
			output_file.Write(&number_of_bytes_per_block, sizeof(int16));

			const int16 bit_depth{ BIT_DEPTH };
			output_file.Write(&bit_depth, sizeof(int16));
		}

		//Add the data chunk.
		{
			constexpr int8 DATA_CHUNK_1[]
			{
				static_cast<int8>('d'),
				static_cast<int8>('a'),
				static_cast<int8>('t'),
				static_cast<int8>('a')
			};
			output_file.Write(DATA_CHUNK_1, sizeof(int8) * 4);

			const int32 data_chunk_size{ static_cast<int32>(resource._Samples[0].Size()) * (static_cast<int32>(resource._Samples.Size()) * BIT_DEPTH / 8) };
			output_file.Write(&data_chunk_size, sizeof(int32));

			DynamicArray<int16> temporary_buffer;
			temporary_buffer.Reserve(resource._Samples[0].Size()* resource._Samples.Size());

			for (uint64 sample_index{ 0 }; sample_index < resource._Samples[0].Size(); ++sample_index)
			{
				for (uint64 channel_index{ 0 }; channel_index < resource._Samples.Size(); ++channel_index)
				{
					switch (BIT_DEPTH)
					{
					case 16:
					{
						temporary_buffer.Emplace(resource._Samples[channel_index][sample_index]);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
					}
				}
			}

			output_file.Write(temporary_buffer.Data(), resource._Samples[0].Size()* resource._Samples.Size() * sizeof(int16));
		}

		//Close the output file.
		output_file.Close();
#else //Use AudioFile implementation.
		AudioFile<float32> audio_file;

		AudioFile<float32>::AudioBuffer audio_buffer;

		audio_buffer.resize(2);

		for (size_t i{ 0 }, size{ audio_buffer.size() }; i < size; ++i)
		{
			audio_buffer[i].reserve(resource._Samples.Size());

			for (const int16 sample : resource._Samples[i])
			{
				audio_buffer[i].emplace_back(static_cast<float32>(sample) / static_cast<float32>(INT16_MAXIMUM));
			}
		}

		audio_file.setAudioBuffer(audio_buffer);
		audio_file.setBitDepth(16);
		audio_file.setSampleRate(static_cast<uint32>(resource._SampleRate));


		audio_file.save(file_path);
#endif

		return true;
	}

};