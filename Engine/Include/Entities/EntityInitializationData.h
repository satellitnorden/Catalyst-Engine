#pragma once

//Core.
#include <Core/General/EngineCore.h>

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
		entity(initialEntity),
		data(initialData),
		force(initialForce)
	{

	}

	//The entity to initialize.
	Entity* RESTRICT entity;

	//The initialization data.
	void* RESTRICT data;

	//Denotes whether or not to force this initialization.
	bool force;

};