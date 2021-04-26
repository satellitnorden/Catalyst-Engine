#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

//Scripting.
#include <Scripting/ScriptingCore.h>

class Script
{

public:

	//Type aliases.
	using InitializeFunction = void(*)(Any<ScriptingConstants::SCRIPT_INTERNAL_STATE_SIZE> *const RESTRICT internal_state);
	using UpdateFunction = void(*)(Any<ScriptingConstants::SCRIPT_INTERNAL_STATE_SIZE> *const RESTRICT internal_state);
	using TerminateFunction = void(*)(Any<ScriptingConstants::SCRIPT_INTERNAL_STATE_SIZE> *const RESTRICT internal_state);

	//The initialize function.
	InitializeFunction _InitializeFunction;

	//The update function.
	UpdateFunction _UpdateFunction;

	//The terminate function.
	TerminateFunction _TerminateFunction;

};