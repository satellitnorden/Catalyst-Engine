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
		FILE_4
	};

	//Enumeration covering all channels.
	enum class Channel : uint8
	{
		RED,
		GREEN,
		BLUE,
		ALPHA
	};

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

	//The first file path.
	const char *RESTRICT _File1{ nullptr };

	//The second file path.
	const char *RESTRICT _File2{ nullptr };

	//The third file path.
	const char *RESTRICT _File3{ nullptr };

	//The fourth file path.
	const char *RESTRICT _File4{ nullptr };

	//The channel mappings.
	StaticArray<ChannelMapping, 4> _ChannelMappings;

	//Whether or not to apply gamma correction.
	bool _ApplyGammaCorrection{ false };

	//The normal map strength. Setting this to 1.0f leaves the texture unaffected.
	float32 _NormalMapStrength{ 1.0f };

	//The number of mipmap levels.
	uint8 _MipmapLevels{ 0 };

};