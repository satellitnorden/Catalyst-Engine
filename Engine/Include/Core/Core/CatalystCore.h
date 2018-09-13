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

//Catalyst core.
#include <Core/Core/CatalystDefines.h>
#include <Core/Core/CatalystMacros.h>
#include <Core/Core/CatalystTypes.h>

//Platform.
#include <Platform/General/Platform.h>

//Enumeration covering all update phases.
enum class UpdatePhase : uint8
{
	OpeningUpdate,
	LogicUpdate,
	RenderUpdate,
	ClosingUpdate
};