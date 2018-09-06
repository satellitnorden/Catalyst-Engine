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
	*	Returns the forward vector of this entity.
	*/
	Vector3 GetForwardVector() const NOEXCEPT;

	/*
	*	Returns the right vector of this entity.
	*/
	Vector3 GetRightVector() const NOEXCEPT;

	/*
	*	Returns the up vector of this entity.
	*/
	Vector3 GetUpVector() const NOEXCEPT;

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