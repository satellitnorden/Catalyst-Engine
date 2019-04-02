#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TextureDataContainer.h>

class TextureData final
{

public:

	//The texture data container.
	TextureDataContainer _TextureDataContainer;

	//The texture format.
	TextureFormat _TextureFormat;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	TextureData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	TextureData(const TextureDataContainer &initialTextureDataContainer, const TextureFormat initialTextureFormat) NOEXCEPT
		:
		_TextureDataContainer(initialTextureDataContainer),
		_TextureFormat(initialTextureFormat)
	{

	}

};