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
		SYSTEM_TERMINATE()
	);

	/*
	*	Default constructor.
	*/
	ComponentSystem() NOEXCEPT
	{

	}

};