#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class PointLightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	PointLightEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(const EntityInitializationData *const RESTRICT data) NOEXCEPT
	{

	}

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT
	{

	}

protected:

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT GetPositionInternal() NOEXCEPT final override;

};