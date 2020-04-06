#pragma once

/*
*	STL includes. Included here so that it is more easily trackable.
*	The goal is to decrease the number of STL dependencies in the future.
*/
#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <thread>
#include <type_traits>

//Catalyst Engine information.
#include <Core/Essential/CatalystEngineInformation.h>

//Catalyst core.
#include <Core/Essential/CatalystDefines.h>
#include <Core/Essential/CatalystMacros.h>
#include <Core/Essential/CatalystTypes.h>

//Platform.
#include <Platform/General/Platform.h>

//Memory.
#include <Memory/Memory.h>

//Functions.
#include <Core/Essential/CatalystFunctions.h>

//Enumeration covering all update phases.
enum class UpdatePhase : uint8
{
	PRE = BIT(0),
	LOGIC = BIT(1),
	PHYSICS = BIT(2),
	RENDER = BIT(3),
	POST = BIT(4),

	NUMBER_OF_UPDATES_PHASES = 5
};

ENUMERATION_BIT_OPERATIONS(UpdatePhase);

/*
*	Returns the index for the given update phase.
*/
FORCE_INLINE uint64 UpdatePhaseIndex(const UpdatePhase phase) NOEXCEPT
{
	switch (phase)
	{
		case UpdatePhase::PRE:
		{
			return 0;
		}

		case UpdatePhase::LOGIC:
		{
			return 1;
		}

		case UpdatePhase::PHYSICS:
		{
			return 2;
		}

		case UpdatePhase::RENDER:
		{
			return 3;
		}

		case UpdatePhase::POST:
		{
			return 4;
		}

		default:
		{
			CRASH();

			return 0;
		}
	}
}