#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

class PointLightEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(PointLightEntity);

	/*
	*	Default constructor.
	*/
	PointLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~PointLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() CATALYST_NOEXCEPT final override;

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
	*	Sets whether or not this point light is enabled.
	*/
	void SetEnabled(const bool newEnabled) CATALYST_NOEXCEPT;

	/*
	*	Sets the attenuation distance.
	*/
	void SetAttenuationDistance(const float newAttenuationDistance) CATALYST_NOEXCEPT;

	/*
	*	Sets the intensity.
	*/
	void SetIntensity(const float newIntensity) CATALYST_NOEXCEPT;

	/*
	*	Sets the color.
	*/
	void SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT;

};