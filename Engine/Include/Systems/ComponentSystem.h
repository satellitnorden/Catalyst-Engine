#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Task.h>

//Forward declarations.
class Component;

class ALIGN(8) ComponentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ComponentSystem);

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

private:

	/*
	*	Updates components for the given update phase.
	*/
	void UpdateComponents(const UpdatePhase update_phase) NOEXCEPT;

};