#pragma once

//Engine core.
#include <EngineCore.h>

class TextureLoader
{

public:

	/*
	*	Given a path, loads a texture and outputs the result in the supplied parameters.
	*/
	static void LoadTexture(const char *RESTRICT texturePath, uint16 &width, uint16 &height, uint8 &numberOfChannels, byte * RESTRICT * RESTRICT data) NOEXCEPT;

	/*
	*	Frees the texture data.
	*/
	static void FreeTexture(byte *RESTRICT data) NOEXCEPT;

};