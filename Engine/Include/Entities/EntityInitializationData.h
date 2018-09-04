#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class Entity;

class EntityInitializationData final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	EntityInitializationData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	EntityInitializationData(Entity* const RESTRICT initialEntity, void* const RESTRICT initialData, const bool initialForce) NOEXCEPT
		:
		_Entity(initialEntity),
		_Data(initialData),
		_Force(initialForce)
	{

	}

	//The entity to initialize.
	Entity* RESTRICT _Entity;

	//The initialization data.
	void* RESTRICT _Data;

	//Denotes whether or not to force this initialization.
	bool _Force;

};