#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class PhysicsEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	PhysicsEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return false;
	}

};