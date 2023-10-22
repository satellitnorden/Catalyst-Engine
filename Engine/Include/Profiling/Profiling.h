#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <ThirdParty/Tracy/tracy/Tracy.hpp>

/*
*	Defines a profiling scope.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define PROFILING_SCOPE(ENTRY_NAME)	ZoneScopedN(#ENTRY_NAME)
	#define PROFILING_FRAME() FrameMark
	#define PROFILING_EVENT(EVENT_NAME) TracyMessageL(#EVENT_NAME)
#else
	#define PROFILING_SCOPE(ENTRY_NAME)
	#define PROFILING_FRAME()
	#define PROFILING_EVENT(EVENT_NAME)
#endif