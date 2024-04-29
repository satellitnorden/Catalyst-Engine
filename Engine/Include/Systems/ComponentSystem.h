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
		ComponentSystem
	);

	/*
	*	Default constructor.
	*/
	ComponentSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the component system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Post-initializes the component system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Terminates the component system.
	*/
	void Terminate() NOEXCEPT;

};