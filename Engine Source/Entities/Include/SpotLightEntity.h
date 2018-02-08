#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

class SpotLightEntity : public Entity
{

public:

	//Universal container of all light entities.
	static DynamicArray<SpotLightEntity *CATALYST_RESTRICT> instances;

	/*
	*	Default constructor.
	*/
	SpotLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~SpotLightEntity() CATALYST_NOEXCEPT;

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
	*	Returns whether or not this spot light is enabled.
	*/
	bool GetEnabled() const CATALYST_NOEXCEPT;

	/*
	*	Sets whether or not this spot light is enabled.
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
	*	Sets the inner cutoff angle.
	*/
	void SetInnerCutoffAngle(const float newInnerCutoffAngle) CATALYST_NOEXCEPT;

	/*
	*	Sets the outer cutoff angle.
	*/
	void SetOuterCutoffAngle(const float newOuterCutoffAngle) CATALYST_NOEXCEPT;

	/*
	*	Sets the color.
	*/
	void SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT;

};