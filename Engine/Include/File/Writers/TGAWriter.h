#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Core/BinaryFile.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

namespace TGAWriterConstants
{
	constexpr int8 TARGA_HEADER_DEFAULT_ID_LENGTH{ 0 };
	constexpr int8 TARGA_HEADER_DEFAULT_COLOUR_MAP_TYPE{ 0 };
	constexpr int16 TARGA_HEADER_DEFAULT_COLOUR_MAP_ORIGIN{ 0 };
	constexpr int16 TARGA_HEADER_DEFAULT_COLOUR_MAP_LENGTH{ 0 };
	constexpr int8 TARGA_HEADER_DEFAULT_COLOUR_MAP_DEPTH{ 0 };
	constexpr int16 TARGA_HEADER_DEFAULT_X_ORIGIN{ 0 };
	constexpr int16 TARGA_HEADER_DEFAULT_Y_ORIGIN{ 0 };

	constexpr int8 HEADER_SIZE{ 18 };

	constexpr int8 DATA_TYPE_CODE_NO_IMAGE_DATA_INCLUDED{ 0 };
	constexpr int8 DATA_TYPE_CODE_UNCOMPRESSED_COLOR_MAPPED_IMAGES{ 1 };
	constexpr int8 DATA_TYPE_CODE_RGB_IMAGES{ 2 };
	constexpr int8 DATA_TYPE_CODE_BLACK_AND_WHITE_IMAGES{ 3 };
	constexpr int8 DATA_TYPE_CODE_RUNLENGTH_ENCODED_COLOR_MAPPED_IMAGES{ 9 };
	constexpr int8 DATA_TYPE_CODE_RUNLENGTH_ENCODED_RGB_IMAGES{ 10 };
	constexpr int8 DATA_TYPE_CODE_COMPRESSED_BLACK_AND_WHITE_IMAGES{ 11 };
	constexpr int8 DATA_TYPE_CODE_COMPRESSED_COLOR_MAPPED_DATA_USING_HUFFMAN_DELTA_AND_RUNLENGTH_ENCODING{ 32 };
	constexpr int8 DATA_TYPE_CODE_COMPRESSED_COLOR_MAPPED_DATA_USING_HUFFMAN_DELTA_AND_RUNLENGTH_ENCODING_4_PASS_QUADTREE_TYPE_PROCESS{ 33 };
}

namespace TGAWriter
{

	/*
	*	Writes the given image to the given file as a .tga file.
	*/
	FORCE_INLINE void Write(const Texture2D<Vector4<float32>> &image, const char *const RESTRICT file) NOEXCEPT
	{
		//Create the output file.
		BinaryFile<IOMode::Out> output_file{ file };

		//Write the header.
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_ID_LENGTH,			sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_ID_LENGTH));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_TYPE,	sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_TYPE));
		output_file.Write(&TGAWriterConstants::DATA_TYPE_CODE_RGB_IMAGES,				sizeof(TGAWriterConstants::DATA_TYPE_CODE_RGB_IMAGES));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_ORIGIN,	sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_ORIGIN));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_LENGTH,	sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_LENGTH));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_DEPTH,	sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_COLOUR_MAP_DEPTH));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_X_ORIGIN,			sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_X_ORIGIN));
		output_file.Write(&TGAWriterConstants::TARGA_HEADER_DEFAULT_Y_ORIGIN,			sizeof(TGAWriterConstants::TARGA_HEADER_DEFAULT_Y_ORIGIN));
		const int16 output_width{ static_cast<int16>(image.GetWidth()) };
		const int16 output_height{ static_cast<int16>(image.GetHeight()) };
		output_file.Write(reinterpret_cast<const int8 *const RESTRICT>(&output_width),	sizeof(output_width));
		output_file.Write(reinterpret_cast<const int8 *const RESTRICT>(&output_height),	sizeof(output_height));
		const uint8 BITS_PER_PIXEL{ 32 };
		output_file.Write(&BITS_PER_PIXEL, sizeof(BITS_PER_PIXEL));
		constexpr int8 IMAGE_DESCRIPTOR{ 0 };
		output_file.Write(&IMAGE_DESCRIPTOR, sizeof(IMAGE_DESCRIPTOR));

		switch (BITS_PER_PIXEL)
		{
			case BITS_PER_PIXEL:
			{
				//Convert the image to the appropriate format.
				Texture2D<Vector4<uint8>> converted_image{ image.GetWidth(), image.GetHeight() };

				for (uint32 Y{ 0 }; Y < image.GetHeight(); ++Y)
				{
					for (uint32 X{ 0 }; X < image.GetWidth(); ++X)
					{
						converted_image.At(X, Y)._R = static_cast<uint8>(image.At(X, Y)._B * UINT8_MAXIMUM);
						converted_image.At(X, Y)._G = static_cast<uint8>(image.At(X, Y)._G * UINT8_MAXIMUM);
						converted_image.At(X, Y)._B = static_cast<uint8>(image.At(X, Y)._R * UINT8_MAXIMUM);
						converted_image.At(X, Y)._A = static_cast<uint8>(image.At(X, Y)._A * UINT8_MAXIMUM);
					}
				}

				//Write the data.
				output_file.Write(converted_image.Data(), converted_image.GetWidth() * converted_image.GetHeight() * sizeof(Vector4<uint8>));

				break;
			}
		}
		

		//Close the output file.
		output_file.Close();
	}
}