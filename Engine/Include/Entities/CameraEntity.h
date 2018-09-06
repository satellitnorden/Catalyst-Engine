#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entity.
#include <Entities/Entity.h>

//Physics.
#include <Physics/PhysicalConstants.h>

class CameraEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	CameraEntity() NOEXCEPT;

	/*
	*	Returns the field of view for this camera entity.
	*/
	float GetFieldOfView() const NOEXCEPT;

	/*
	*	Returns the near plane of the frustum.
	*/
	float GetNearPlane() const NOEXCEPT;

	/*
	*	Returns the far plane of the frustum.
	*/
	float GetFarPlane() const NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

};