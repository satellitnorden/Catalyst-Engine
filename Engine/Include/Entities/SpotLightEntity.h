#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class SpotLightEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(SpotLightEntity);

	/*
	*	Default constructor.
	*/
	SpotLightEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~SpotLightEntity() NOEXCEPT;

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
	*	Returns whether or not this spot light is enabled.
	*/
	bool GetEnabled() const NOEXCEPT;

	/*
	*	Sets whether or not this spot light is enabled.
	*/
	void SetEnabled(const bool newEnabled) NOEXCEPT;

	/*
	*	Sets the attenuation distance.
	*/
	void SetAttenuationDistance(const float newAttenuationDistance) NOEXCEPT;

	/*
	*	Sets the intensity.
	*/
	void SetIntensity(const float newIntensity) NOEXCEPT;

	/*
	*	Sets the inner cutoff angle.
	*/
	void SetInnerCutoffAngle(const float newInnerCutoffAngle) NOEXCEPT;

	/*
	*	Sets the outer cutoff angle.
	*/
	void SetOuterCutoffAngle(const float newOuterCutoffAngle) NOEXCEPT;

	/*
	*	Sets the color.
	*/
	void SetColor(const Vector3 &newColor) NOEXCEPT;

};