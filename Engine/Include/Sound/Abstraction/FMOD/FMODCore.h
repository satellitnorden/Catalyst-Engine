#if defined(CATALYST_SOUND_FMOD)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Thirdy party.
#include <ThirdParty/fmod_studio.hpp>

/*
*	Turns FMOD debugging on/off.
*/
#if defined(CATALYST_CONFIGURATION_DEBUG)
	#define FMOD_DEBUGGING true
#else
	#define FMOD_DEBUGGING false
#endif

/*
*	Checks the result of an FMOD function call and takes appropriate measures if it fails.
*/
#if FMOD_DEBUGGING
	#define FMOD_ERROR_CHECK(function) { const FMOD_RESULT result{ function }; if (result != FMOD_OK) ASSERT(false, "FMOD function call failed."); }
#else
	#define FMOD_ERROR_CHECK(function) function
#endif
#endif