#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TextureDataContainer.h>

class TextureData final
{

public:

	//The texture data container.
	TextureDataContainer _TextureDataContainer;

	//Defines the address mode.
	AddressMode _AddressMode;

	//Defines the magnification filter.
	TextureFilter _MagnificationFilter;

	//Defines the mipmap mode.
	MipmapMode _MipmapMode;

	//The texture format.
	TextureFormat _TextureFormat;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	TextureData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	TextureData(const TextureDataContainer &initialTextureDataContainer, const AddressMode initialAddressMode, const TextureFilter initialMagnificationFilter, const MipmapMode initialMipmapMode, const TextureFormat initialTextureFormat) NOEXCEPT
		:
		_TextureDataContainer(initialTextureDataContainer),
		_AddressMode(initialAddressMode),
		_MagnificationFilter(initialMagnificationFilter),
		_MipmapMode(initialMipmapMode),
		_TextureFormat(initialTextureFormat)
	{

	}

};