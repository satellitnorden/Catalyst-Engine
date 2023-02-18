#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Profiling.
#include <Profiling/ProfilingScope.h>

/*
*	Defines a profiling scope.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define PROFILING_SCOPE(ENTRY_NAME)																		\
	static thread_local ProfilingScopeStaticData ENTRY_NAME##_PROFILING_SCOPE_STATIC_DATA;					\
	ProfilingScope ENTRY_NAME##_PROFILING_SCOPE{ #ENTRY_NAME, &ENTRY_NAME##_PROFILING_SCOPE_STATIC_DATA };
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
#endif