#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ModelBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output;

	//The resource id.
	const char *RESTRICT _ID;

	//The file.
	const char *RESTRICT _File;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr ModelBuildParameters() NOEXCEPT
		:
		_Output(nullptr),
		_ID(nullptr),
		_File(nullptr)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr ModelBuildParameters(const char *const RESTRICT initialOutput,
												const char *const RESTRICT initialID,
												const char *const RESTRICT initialFile) NOEXCEPT
		:
		_Output(initialOutput),
		_ID(initialID),
		_File(initialFile)
	{

	}

};