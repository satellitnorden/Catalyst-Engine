#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

class DirectionalLightEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(DirectionalLightEntity);

	/*
	*	Default constructor.
	*/
	DirectionalLightEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~DirectionalLightEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() NOEXCEPT final override;

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
	*	Sets the intensity of this directional light entity.
	*/
	void SetIntensity(const float newIntensity) NOEXCEPT;

	/*
	*	Sets the color of this directional light entity.
	*/
	void SetColor(const Vector3 &newColor) NOEXCEPT;

};