#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class DynamicPhysicalEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	DynamicPhysicalEntity() NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetScaleInternal() NOEXCEPT final override;

};