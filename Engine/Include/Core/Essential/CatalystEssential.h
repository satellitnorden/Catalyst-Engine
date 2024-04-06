#pragma once

/*
*	Pre-defines - should be included first.
*/
#include <Core/Essential/CatalystPreDefines.h>

/*
*	Compiler-specific defines.
*/
#if defined(CATALYST_MSVC)
	/*
	*	Disable warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
	*	This is because STL libraries use exceptions, but exceptions are usually turned off in game builds.
	*/
	#pragma warning(disable: 4530)

	/*
	*	Improve the inlining behavior of the compiler.
	*/
	#pragma inline_depth(255)
	#pragma inline_recursion(on)
	#pragma auto_inline(on)
#endif

//Catalyst Engine information.
#include <Core/Essential/CatalystEngineInformation.h>

//Catalyst core.
#include <Core/Essential/CatalystDefines.h>
#include <Core/Essential/CatalystMacros.h>
#include <Core/Essential/CatalystTypes.h>

//Compiler.
#include <Core/Compiler/Compiler.h>

//Platform.
#include <Platform/General/CatalystPlatform.h>

//Memory.
#include <Memory/Memory.h>

//Functions.
#include <Core/Essential/CatalystFunctions.h>

//Enumeration covering all update phases.
enum class UpdatePhase : uint16
{
	/*
	*	During this update phase, certain systems are reset and prepared for the coming frame.
	*/
	PRE = BIT(0),

	/*
	*	During this update phase, entities are initialized/terminated/destroyed.
	*/
	ENTITY = BIT(1),

	/*
	*	During this update phase, input is retrieved from the platform.
	*/
	INPUT = BIT(2),

	/*
	*	During this update phase, gameplay logic is simulated.
	*/
	GAMEPLAY = BIT(3),

	/*
	*	During this update phase, user interface elements are updated.
	*/
	USER_INTERFACE = BIT(4),

	/*
	*	During this update phase, physics is simulated.
	*/
	PHYSICS = BIT(5),

	/*
	*	During this update phase, the game world is being prepared for rendering.
	*/
	PRE_RENDER = BIT(6),

	/*
	*	During this update phase, the game world is rendered.
	*/
	RENDER = BIT(7),

	/*
	*	During this update phase, certain systems clean up data that is no longer needed.
	*/
	POST = BIT(8),

	/*
	*	The total number of update phases.
	*/
	NUMBER_OF_UPDATES_PHASES = 9
};

ENUMERATION_BIT_OPERATIONS(UpdatePhase);