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
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The number of mipmap levels.
	uint8 _MipmapLevels{ 0 };

	//The mode.
	Mode _Mode{ Mode::RToRGBA };

	//The file path for the R channel.
	const char *RESTRICT _FileR{ nullptr };

	//The file path for the G channel.
	const char *RESTRICT _FileG{ nullptr };

	//The file path for the B channel.
	const char *RESTRICT _FileB{ nullptr };

	//The file path for the A channel.
	const char *RESTRICT _FileA{ nullptr };

	//Whether or not to apply gamma correction.
	bool _ApplyGammaCorrection{ false };

};