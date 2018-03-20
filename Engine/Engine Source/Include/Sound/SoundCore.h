#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Third party libraries.
#include <Third Party Libraries/fmod_studio.hpp>

/*
*	Type aliases.
*/
using EventDescription = FMOD::Studio::EventDescription;

#if !defined(CATALYST_FINAL)
/*
*	Given an FMOD_RESULT, print the result.
*/
static void PrintFMODResultAndBreak(const FMOD_RESULT result) NOEXCEPT
{
	switch (result)
	{
		default:
		{
			PRINT_TO_CONSOLE("Undefined FMOD result");

			break;
		}

		case FMOD_ERR_EVENT_NOTFOUND:
		{
			PRINT_TO_CONSOLE("FMOD result was FMOD_ERR_EVENT_NOTFOUND");

			break;
		}

		case FMOD_ERR_INVALID_PARAM:
		{
			PRINT_TO_CONSOLE("FMOD result was FMOD_ERR_INVALID_PARAM");

			break;
		}
	}

	BREAKPOINT;
}
#endif

/*
*	Checks the result of FMOD functions and takes appropriate action in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#define FMOD_ERROR_CHECK(function) { const FMOD_RESULT result{ function }; if (result != FMOD_OK) { PrintFMODResultAndBreak(result); } }
#else
	#define FMOD_ERROR_CHECK(function) function;
#endif