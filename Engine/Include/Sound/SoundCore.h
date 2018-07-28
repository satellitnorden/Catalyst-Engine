#pragma once

//Core.
#include <Core/EngineCore.h>

#if defined(CATALYST_WINDOWS)
//Third party libraries.
#include <Third Party Libraries/fmod_studio.hpp>
#endif

#if !defined(CATALYST_FINAL) && defined(CATALYST_WINDOWS)
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
#if !defined(CATALYST_FINAL) && defined(CATALYST_WINDOWS)
	#define FMOD_ERROR_CHECK(function) { const FMOD_RESULT result{ function }; if (result != FMOD_OK) { PrintFMODResultAndBreak(result); } }
#else
	#define FMOD_ERROR_CHECK(function) function;
#endif