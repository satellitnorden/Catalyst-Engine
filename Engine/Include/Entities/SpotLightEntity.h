#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class SpotLightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	SpotLightEntity() NOEXCEPT;

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

protected:

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

};