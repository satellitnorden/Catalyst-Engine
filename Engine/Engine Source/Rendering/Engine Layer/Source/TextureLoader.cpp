//Header file.
#include <TextureLoader.h>

//Third party libraries.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/*
*	Given a path, loads a texture and outputs the result in the supplied parameters.
*/
void TextureLoader::LoadTexture(const char *RESTRICT texturePath, uint16 &width, uint16 &height, uint8 &numberOfChannels, byte * RESTRICT * RESTRICT data) NOEXCEPT
{
	//Load the texture data.
	int tempWidth, tempHeight, tempNumberOfChannels;
	*data = stbi_load(texturePath, &tempWidth, &tempHeight, &tempNumberOfChannels, STBI_rgb_alpha);
	width = static_cast<uint16>(tempWidth);
	height = static_cast<uint16>(tempHeight);
	numberOfChannels = static_cast<uint8>(tempNumberOfChannels);
}

/*
*	Frees the texture data.
*/
void TextureLoader::FreeTexture(byte *RESTRICT data) NOEXCEPT
{
	//Free the texture data.
	stbi_image_free(data);
}