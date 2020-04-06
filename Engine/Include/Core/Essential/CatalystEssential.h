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
	/*
	*	During this update phase, input is retrieved from the platform.
	*/
	INPUT,

	/*
	*	During this update phase, user interface elements are updated.
	*/
	USER_INTERFACE,

	/*
	*	During this update phase, game logic is simulated.
	*/
	LOGIC,

	/*
	*	During this update phase, physics is simulated.
	*/
	PHYSICS,

	/*
	*	During this update phase, the game world is rendered.
	*/
	RENDER,

	/*
	*	During this update phase, sound is calculated.
	*/
	POST,

	/*
	*	The total number of update phases.
	*/
	NUMBER_OF_UPDATES_PHASES
};