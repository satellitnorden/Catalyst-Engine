#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Profiling.
#if !defined(INCLUDED_FROM_PROFILING_SYSTEM)
#include <Profiling/ProfilingTimer.h>
#endif

/*
*	Turns profiling on off.
*/
#if /*defined(CATALYST_CONFIGURATION_DEBUG) || */defined(CATALYST_CONFIGURATION_PROFILE)
	#define PROFILING_ENABLED
#endif

/*
*	Defines a profiling scope.
*/
#if defined(PROFILING_ENABLED)
	#define PROFILING_SCOPE(ENTRY_NAME) ProfilingTimer profilingTimer{ ENTRY_NAME };
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
#endif