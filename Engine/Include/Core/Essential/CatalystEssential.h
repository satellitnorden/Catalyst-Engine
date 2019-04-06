#pragma once

/*
*	STL includes. Included here so that it is more easily trackable.
*	The goal is to decrease the number of STL dependencies in the future.
*/
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <random>
#include <set>
#include <sstream>
#include <thread>
#include <type_traits>

//Catalyst core.
#include <Core/Essential/CatalystDefines.h>
#include <Core/Essential/CatalystFunctions.h>
#include <Core/Essential/CatalystMacros.h>
#include <Core/Essential/CatalystTypes.h>

//Platform.
#include <Platform/General/Platform.h>

//Memory.
#include <Memory/Memory.h>

//Enumeration covering all update phases.
enum class UpdatePhase : uint8
{
	Pre = BIT(0),
	Logic = BIT(1),
	Physics = BIT(2),
	Render = BIT(3),
	Post = BIT(4)
};

ENUMERATION_BIT_OPERATIONS(UpdatePhase);

/*
*	Returns the current update phase.
*/
RESTRICTED static NO_DISCARD UpdatePhase *const RESTRICT CurrentUpdatePhase() NOEXCEPT
{
	extern UpdatePhase CURRENT_UPDATE_PHASE;

	return &CURRENT_UPDATE_PHASE;
}

/*
*	Function for specifying during which update phase(s) read access is allowed to a specific type. Needs to be specialized on a per type basis.
*/
template <typename TYPE>
static NO_DISCARD UpdatePhase AllowedReadAccess() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Logic | UpdatePhase::Physics | UpdatePhase::Render | UpdatePhase::Post;
}

/*
*	Function for specifying during which update phase(s) write access is allowed to a specific type. Needs to be specialized on a per type basis.
*/
template <typename TYPE>
static NO_DISCARD UpdatePhase AllowedWriteAccess() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Logic | UpdatePhase::Physics | UpdatePhase::Render | UpdatePhase::Post;
}

/*
*	Verifies read access for a given type during the current update phase.
*/
template <typename TYPE>
void VerifyReadAccess() NOEXCEPT
{
	ASSERT(TEST_BIT(AllowedReadAccess<TYPE>(), *CurrentUpdatePhase()), "Read access is not allowed for this type during this update phase!");
}

/*
*	Verifies write access for a given type during the current update phase.
*/
template <typename TYPE>
void VerifyWriteAccess() NOEXCEPT
{
	ASSERT(TEST_BIT(AllowedWriteAccess<TYPE>(), *CurrentUpdatePhase()), "Write access is not allowed for this type during this update phase!");
}