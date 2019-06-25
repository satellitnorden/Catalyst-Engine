#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Profiling.
#if !defined(INCLUDED_FROM_PROFILING_SYSTEM)
#include <Profiling/ProfilingTimer.h>
#endif

/*
*	Defines a profiling scope.
*/
#if defined(CATALYST_CONFIGURATION_PROFILE)
	#define PROFILING_SCOPE(ENTRY_NAME) ProfilingTimer profilingTimer{ ENTRY_NAME };
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
#endif