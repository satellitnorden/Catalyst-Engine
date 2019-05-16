#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class LightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	LightEntity() NOEXCEPT;

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

	/*
	*	Returns the position of this light.
	*/
	Vector3<float> GetPosition() NOEXCEPT;

	/*
	*	Sets the position of this light.
	*/
	void SetPosition(const Vector3<float> &position) NOEXCEPT;

	/*
	*	Returns the strength of this light.
	*/
	float GetStrength() NOEXCEPT;

	/*
	*	Sets the strength of this light.
	*/
	void SetStrength(const float strength) NOEXCEPT;

};