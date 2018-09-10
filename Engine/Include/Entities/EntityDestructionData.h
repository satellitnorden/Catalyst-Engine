#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class Entity;

class EntityDestructionData final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	EntityDestructionData() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	EntityDestructionData(Entity* const RESTRICT initialEntity, const bool initialForce) NOEXCEPT
		:
		_Entity(initialEntity),
		_Force(initialForce)
	{

	}

	//The entity to destroy.
	Entity* RESTRICT _Entity;

	//Denotes whether or not to force this destruction.
	bool _Force;

};
