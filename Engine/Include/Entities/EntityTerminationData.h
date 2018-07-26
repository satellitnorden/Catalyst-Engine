#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
		entity(initialEntity),
		force(initialForce)
	{

	}

	//The entity to terminate.
	Entity* RESTRICT entity;

	//Denotes whether or not to force this termination.
	bool force;

};