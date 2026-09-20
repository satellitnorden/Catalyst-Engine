#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//File.
#include <File/Core/BinaryOutputFile.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>

class WAVWriter final
{

public:

	/*
	*	Writes the sound asset to the given file path. Returns if the write was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Write(const char *const RESTRICT file_path, const AudioStream &audio_stream) NOEXCEPT
	{
		//Open the output file.
		BinaryOutputFile output_file{ file_path };

		//Cache the bit depth.
		const uint8 bit_depth{ Audio::BitsPerSample(audio_stream.GetFormat()) };

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

			const int32 file_size_in_bytes{ 4 + 24 + 8 + (static_cast<int32>(audio_stream.GetNumberOfSamples()) * (static_cast<int32>(audio_stream.GetNumberOfChannels()) * bit_depth / 8)) };
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

			const int16 audio_format{ audio_stream.GetFormat() == Audio::Format::FLOAT_32_BIT ? 3 : 1 };
			output_file.Write(&audio_format, sizeof(int16));

			const int16 number_of_channels{ static_cast<int16>(audio_stream.GetNumberOfChannels()) };
			output_file.Write(&number_of_channels, sizeof(int16));

			const int32 sample_rate{ static_cast<int32>(audio_stream.GetSampleRate()) };
			output_file.Write(&sample_rate, sizeof(int32));

			const int32 number_of_bytes_per_second{ (number_of_channels * sample_rate * bit_depth) / 8 };
			output_file.Write(&number_of_bytes_per_second, sizeof(int32));

			const int16 number_of_bytes_per_block{ static_cast<int16>(number_of_channels * (bit_depth / 8)) };
			output_file.Write(&number_of_bytes_per_block, sizeof(int16));

			const int16 bits_per_sample{ static_cast<int16>(bit_depth) };
			output_file.Write(&bits_per_sample, sizeof(int16));
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

			const int32 data_chunk_size{ static_cast<int32>(audio_stream.GetNumberOfSamples()) * (static_cast<int32>(audio_stream.GetNumberOfChannels()) * bit_depth / 8) };
			output_file.Write(&data_chunk_size, sizeof(int32));

			output_file.Write(audio_stream.GetData(), audio_stream.GetNumberOfSamples() * audio_stream.GetNumberOfChannels() * bit_depth / 8);
		}

		//Close the output file.
		output_file.Close();

		return true;
	}

};