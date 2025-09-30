#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Task.h>

//Systems.
#include <Systems/System.h>

//Forward declarations.
class Component;

class ComponentSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ComponentSystem,
		SYSTEM_INITIALIZE()
		SYSTEM_POST_INITIALIZE()
		SYSTEM_UPDATE(RANGE(PRE, RUN_ON_MAIN_THREAD), RANGE(INPUT, RUN_ON_MAIN_THREAD), RANGE(GAMEPLAY, RUN_ON_MAIN_THREAD), RANGE(USER_INTERFACE, RUN_ON_MAIN_THREAD), RANGE(PHYSICS, RUN_ON_MAIN_THREAD), RANGE(PRE_RENDER, RUN_ON_MAIN_THREAD), RANGE(RENDER, RUN_ON_MAIN_THREAD), RANGE(POST, RUN_ON_MAIN_THREAD))
		SYSTEM_TERMINATE()
	);

	/*
	*	Default constructor.
	*/
	ComponentSystem() NOEXCEPT
	{

	}

};