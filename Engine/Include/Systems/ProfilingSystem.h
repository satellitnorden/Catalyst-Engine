#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>
#include <Core/General/UpdateContext.h>

//Profiling.
#define INCLUDED_FROM_PROFILING_SYSTEM
#include <Profiling/ProfilingCore.h>
#undef INCLUDED_FROM_PROFILING_SYSTEM

class ProfilingSystem final
{

public:

	/*
	*	Updates the profiling system during the physics update phase.
	*/
	static void PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the profiling system during the post update phase.
	*/
	static void PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Adds a profiling entry.
	*/
	static void AddProfilingEntry(const DynamicString &name, const float duration) NOEXCEPT;

};