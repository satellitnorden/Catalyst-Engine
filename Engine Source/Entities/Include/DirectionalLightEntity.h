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
	DirectionalLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~DirectionalLightEntity() CATALYST_NOEXCEPT;

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
	*	Sets the intensity of this directional light entity.
	*/
	void SetIntensity(const float newIntensity) CATALYST_NOEXCEPT;

	/*
	*	Sets the color of this directional light entity.
	*/
	void SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT;

};