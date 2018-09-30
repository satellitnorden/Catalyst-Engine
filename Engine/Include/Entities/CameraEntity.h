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
	*	Returns the field of view in degrees for this camera entity.
	*/
	float GetFieldOfViewDegrees() const NOEXCEPT;

	/*
	*	Returns the field of view in radians for this camera entity.
	*/
	float GetFieldOfViewRadians() const NOEXCEPT;

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
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

	/*
	*	Callback for when this entity is moved.
	*/
	void OnMove() NOEXCEPT final override;

	/*
	*	Callback for when this entity is rotated.
	*/
	void OnRotate() NOEXCEPT final override;

};