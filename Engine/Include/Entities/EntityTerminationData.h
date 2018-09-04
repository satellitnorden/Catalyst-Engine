#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class Entity;

class EntityTerminationData final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	EntityTerminationData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	EntityTerminationData(Entity* const RESTRICT initialEntity, const bool initialForce) NOEXCEPT
		:
		_Entity(initialEntity),
		_Force(initialForce)
	{

	}

	//The entity to terminate.
	Entity* RESTRICT _Entity;

	//Denotes whether or not to force this termination.
	bool _Force;

};