#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Profiling.
#include <Profiling/ProfilingScope.h>

/*
*	Defines a profiling scope.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define PROFILING_SCOPE(ENTRY_NAME) ProfilingScope profiling_scope{ ENTRY_NAME };
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
#endif