#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration covering all texture filters.
enum class TextureFilter : uint8
{
	Linear,
	Nearest
};

//Enumeration covering all mipmap modes.
enum class MipmapMode : uint8
{
	Linear,
	Nearest
};

class TextureCreationParameters final
{

public:

	/*
	*	Default constructor.
	*/
	TextureCreationParameters() NOEXCEPT
		:
		magnificationFilter(TextureFilter::Nearest),
		mipmapMode(MipmapMode::Nearest)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	TextureCreationParameters(const TextureFilter initialMagnificationFilter, const MipmapMode initialMipmapMode) NOEXCEPT
		:
		magnificationFilter(initialMagnificationFilter),
		mipmapMode(initialMipmapMode)
	{

	}

	//Defines the magnification filter.
	TextureFilter magnificationFilter{ TextureFilter::Nearest };

	//Defines the mipmsp mode.
	MipmapMode mipmapMode{ MipmapMode::Nearest };

};