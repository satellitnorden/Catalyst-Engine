#pragma once

//Engine core.
#include <EngineCore.h>

//Entity.
#include <Entity.h>

//Physics.
#include <PhysicalConstants.h>

class CameraEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(CameraEntity);

	/*
	*	Default constructor.
	*/
	CameraEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~CameraEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	Vector3& GetPosition() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	Vector3& GetRotation() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	Vector3& GetScale() CATALYST_NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT final override;

	/*
	*	Returns the forward vector of this entity.
	*/
	Vector3 GetForwardVector() CATALYST_NOEXCEPT;

	/*
	*	Returns the right vector of this entity.
	*/
	Vector3 GetRightVector() CATALYST_NOEXCEPT;

	/*
	*	Returns the up vector of this entity.
	*/
	Vector3 GetUpVector() CATALYST_NOEXCEPT;

	/*
	*	Returns the field of view for this camera entity.
	*/
	float GetFieldOfView() const CATALYST_NOEXCEPT;

	/*
	*	Returns the near plane of the frustum.
	*/
	float GetNearPlane() const CATALYST_NOEXCEPT;

	/*
	*	Returns the far plane of the frustum.
	*/
	float GetFarPlane() const CATALYST_NOEXCEPT;

};