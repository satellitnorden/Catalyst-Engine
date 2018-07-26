#pragma once

//Core.
#include <Core/EngineCore.h>

//Entity.
#include <Entities/Entity.h>

//Physics.
#include <Physics/PhysicalConstants.h>

class CameraEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(CameraEntity);

	/*
	*	Default constructor.
	*/
	CameraEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~CameraEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() const NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() const NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() const NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) NOEXCEPT final override;

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

};