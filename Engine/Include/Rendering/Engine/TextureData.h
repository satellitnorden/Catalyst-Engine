#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TextureDataContainer.h>

class TextureData final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	TextureData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	TextureData(const TextureDataContainer &initialTextureDataContainer, const AddressMode initialAddressMode, const TextureFilter initialMagnificationFilter, const MipmapMode initialMipmapMode, const TextureFormat initialTextureFormat) NOEXCEPT
		:
		textureDataContainer(initialTextureDataContainer),
		addressMode(initialAddressMode),
		magnificationFilter(initialMagnificationFilter),
		mipmapMode(initialMipmapMode),
		textureFormat(initialTextureFormat)
	{

	}

	//The texture data container.
	TextureDataContainer textureDataContainer;

	//Defines the address mode.
	AddressMode addressMode;

	//Defines the magnification filter.
	TextureFilter magnificationFilter;

	//Defines the mipmap mode.
	MipmapMode mipmapMode;

	//The texture format.
	TextureFormat textureFormat;

};