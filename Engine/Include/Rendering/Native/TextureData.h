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

	//The texture usage.
	TextureUsage _TextureUsage;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	TextureData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	TextureData(TextureDataContainer &&initial_texture_data_container, const TextureFormat initial_texture_format, const TextureUsage initial_texture_usage) NOEXCEPT
		:
		_TextureDataContainer(std::move(initial_texture_data_container)),
		_TextureFormat(initial_texture_format),
		_TextureUsage(initial_texture_usage)
	{

	}

};