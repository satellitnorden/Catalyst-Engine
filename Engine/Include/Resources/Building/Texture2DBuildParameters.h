#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

class Texture2DBuildParameters final
{

public:

	//Enumeration covering all files.
	enum class File : uint8
	{
		FILE_1,
		FILE_2,
		FILE_3,
		FILE_4,
		DEFAULT //Special value for when no file should be used, but the default value specified.
	};

	//Enumeration covering all channels.
	enum class Channel : uint8
	{
		RED,
		GREEN,
		BLUE,
		ALPHA
	};

	//Transform function type alias.
	using TransformFunction = void(*)(const uint32 X, const uint32 Y, Vector4<float32> *const RESTRICT texel);

	/*
	*	Channel mapping class definition.
	*/
	class ChannelMapping final
	{

	public:

		//The file.
		File _File;

		//The channel.
		Channel _Channel;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE ChannelMapping() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE ChannelMapping(const File initial_file, const Channel initial_channel) NOEXCEPT
			:
			_File(initial_file),
			_Channel(initial_channel)
		{

		}

	};

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The default width.
	uint32 _DefaultWidth;

	//The default height.
	uint32 _DefaultHeight;

	//The first file path.
	const char *RESTRICT _File1{ nullptr };

	//The second file path.
	const char *RESTRICT _File2{ nullptr };

	//The third file path.
	const char *RESTRICT _File3{ nullptr };

	//The fourth file path.
	const char *RESTRICT _File4{ nullptr };

	//The default value.
	Vector4<float32> _Default;

	//The channel mappings.
	StaticArray<ChannelMapping, 4> _ChannelMappings;

	//Whether or not to apply gamma correction.
	bool _ApplyGammaCorrection{ false };

	//The transform function.
	TransformFunction _TransformFunction;

	//The base mipmap level.
	uint8 _BaseMipmapLevel{ 0 };

	//The number of mipmap levels.
	uint8 _MipmapLevels{ 1 };

};