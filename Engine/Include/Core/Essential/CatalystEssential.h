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