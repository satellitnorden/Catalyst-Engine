#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Third party.
#include <ThirdParty/stb_image.h>

namespace PNGReader
{

	/*
	*	Reads the image at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file, Texture2D<Vector4<float32>> *const RESTRICT texture) NOEXCEPT
	{
		//Load the texture data.
		int32 width, height, number_of_channels;
		uint8 *RESTRICT data{ stbi_load(file, &width, &height, &number_of_channels, STBI_rgb_alpha) };

		//Load the texture data into the texture.
		texture->Initialize(static_cast<uint32>(width), static_cast<uint32>(height));

		for (uint32 Y{ 0 }; Y < texture->GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < texture->GetWidth(); ++X)
			{
				//Calculate the texel.
				Vector4<float32> &texel{ texture->At(X, Y) };

				for (uint8 i{ 0 }; i < 4; ++i)
				{
					texel[i] = static_cast<float32>(data[(X + (Y * texture->GetWidth())) * 4 + i]) / static_cast<float32>(UINT8_MAXIMUM);
				}
			}
		}

		//Free the texture data.
		stbi_image_free(data);

		return true;
	}

}