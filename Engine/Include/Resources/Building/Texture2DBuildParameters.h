#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Texture2DBuildParameters final
{

public:

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		/*
		*	Uses the file specified for the R channel for the R, G, B and A channels.
		*/
		RToRGBA,

		/*
		*	Uses the file specified for the R channel for the R, G and A channels
		*	and the file specified for the A channel for the A channel.
		*/
		RToRGBAToA,

		/*
		*	Uses the file specified for the R channel for the R channel,
		*	the file specified for the G channel for the G channel,
		*	the file specified for the B channel for the B channel
		*	and the file specified for the A channel for the A channel.
		*/
		RToRGToGBToBAToA
	};

	//The output file path.
	const char *RESTRICT _Output;

	//The resource id.
	const char *RESTRICT _ID;

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The mode.
	Mode _Mode;

	//The file path for the R channel.
	const char *RESTRICT _FileR;

	//The file path for the G channel.
	const char *RESTRICT _FileG;

	//The file path for the B channel.
	const char *RESTRICT _FileB;

	//The file path for the A channel.
	const char *RESTRICT _FileA;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Texture2DBuildParameters() NOEXCEPT
		:
		_Output(nullptr),
		_ID(nullptr),
		_MipmapLevels(0),
		_Mode(Mode::RToRGBA),
		_FileR(nullptr),
		_FileG(nullptr),
		_FileB(nullptr),
		_FileA(nullptr)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Texture2DBuildParameters(const char *const RESTRICT initialOutput,
													const char *const RESTRICT initialID,
													const uint8 initialMipmapLevels,
													const Mode initialMode,
													const char *const RESTRICT initialFileR,
													const char *const RESTRICT initialFileG,
													const char *const RESTRICT initialFileB,
													const char *const RESTRICT initialFileA) NOEXCEPT
		:
		_Output(initialOutput),
		_ID(initialID),
		_MipmapLevels(initialMipmapLevels),
		_Mode(initialMode),
		_FileR(initialFileR),
		_FileG(initialFileR),
		_FileB(initialFileR),
		_FileA(initialFileR)
	{

	}

};