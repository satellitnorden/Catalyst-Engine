#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <TextureDataContainer.h>

//Enumeration covering all mipmap modes.
enum class MipmapMode : uint8
{
	Linear,
	Nearest
};

//Enumeration covering all texture filters.
enum class TextureFilter : uint8
{
	Linear,
	Nearest
};

//Enumeration covering all texture formats.
enum class TextureFormat : uint8
{
	R8_Byte,
	R8G8B8A8_Byte,
	R32G32B32A32_Float
};

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
	TextureData(const TextureDataContainer &initialTextureDataContainer, const TextureFilter initialMagnificationFilter, const MipmapMode initialMipmapMode, const TextureFormat initialTextureFormat) NOEXCEPT
		:
		textureDataContainer(initialTextureDataContainer),
		magnificationFilter(initialMagnificationFilter),
		mipmapMode(initialMipmapMode),
		textureFormat(initialTextureFormat)
	{

	}

	//The texture data container.
	TextureDataContainer textureDataContainer;

	//Defines the magnification filter.
	TextureFilter magnificationFilter;

	//Defines the mipmsp mode.
	MipmapMode mipmapMode;

	//The texture format.
	TextureFormat textureFormat;

};