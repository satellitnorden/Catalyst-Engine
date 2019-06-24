#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Turns profiling on off.
*/
#if defined(CATALYST_CONFIGURATION_DEBUG) || defined(CATALYST_CONFIGURATION_PROFILE)
	#define PROFILINGE_ENABLED true
#else
	#define PROFILINGE_ENABLED false
#endif

/*
*	Defines a profiling scope.
*/
#if PROFILING_ENABLED
	#define PROFILING_SCOPE(ENTRY_NAME)
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
#endif