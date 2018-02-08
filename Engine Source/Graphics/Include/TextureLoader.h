#pragma once

//Engine core.
#include <EngineCore.h>

//Third party libraries.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TextureLoader
{

	/*
	*	Given a path, loads a texture and outputs the result in the supplied parameters.
	*/
	void LoadTexture(const char *RESTRICT texturePath, int &width, int &height, int&numberOfChannels, byte * RESTRICT * RESTRICT data) NOEXCEPT
	{
		//Load the texture data.
		*data = stbi_load(texturePath, &width, &height, &numberOfChannels, STBI_rgb_alpha);
	}

	/*
	*	Frees the texture data.
	*/
	void FreeTexture(byte *RESTRICT data) NOEXCEPT
	{
		//Free the texture data.
		stbi_image_free(data);
	}

}