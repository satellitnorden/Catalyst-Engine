#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Core/BinaryFile.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Third Party.
#include <ThirdParty/lodepng.h>

namespace PNGWriter
{

	/*
	*	Writes the given image to the given file as a .tga file.
	*/
	FORCE_INLINE static void Write(const Texture2D<Vector4<float32>> &image, const char *const RESTRICT file_path) NOEXCEPT
	{
		//Create the png buffer.
		std::vector<uint8> png_buffer;
		png_buffer.reserve(image.GetWidth() * image.GetHeight() * 4);

		for (uint32 Y{ 0 }; Y < image.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < image.GetWidth(); ++X)
			{
				png_buffer.emplace_back(static_cast<uint8>(image.At(X, image.GetHeight() - 1 - Y)._R * static_cast<float32>(UINT8_MAXIMUM)));
				png_buffer.emplace_back(static_cast<uint8>(image.At(X, image.GetHeight() - 1 - Y)._G * static_cast<float32>(UINT8_MAXIMUM)));
				png_buffer.emplace_back(static_cast<uint8>(image.At(X, image.GetHeight() - 1 - Y)._B * static_cast<float32>(UINT8_MAXIMUM)));
				png_buffer.emplace_back(static_cast<uint8>(image.At(X, image.GetHeight() - 1 - Y)._A * static_cast<float32>(UINT8_MAXIMUM)));
			}
		}

		//Encode.
		std::vector<uint8> image_buffer;
		lodepng::encode(image_buffer, png_buffer, image.GetWidth(), image.GetHeight());

		//Save.
		lodepng::save_file(image_buffer, file_path);
	}

}