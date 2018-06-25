#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	EntityInitializationData(const Entity* const RESTRICT initialEntity, const void* const RESTRICT initialData, const bool initialForce) NOEXCEPT
		:
		entity(initialEntity),
		data(initialData),
		force(initialForce)
	{

	}

	//The entity to initialize.
	const Entity* const RESTRICT entity;

	//The initialization data.
	const void* const RESTRICT data;

	//Denotes whether or not to force this initialization.
	const bool force;

};