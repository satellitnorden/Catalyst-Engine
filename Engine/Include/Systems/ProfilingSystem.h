#if defined(CATALYST_CONFIGURATION_PROFILE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

class ProfilingSystem final
{

public:

	/*
	*	Updates the profiling system during the pre update phase.
	*/
	static void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

};
#endif