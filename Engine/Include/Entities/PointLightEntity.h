#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class PointLightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	PointLightEntity() NOEXCEPT;

	/*
	*	Sets whether or not this point light is enabled.
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
	*	Sets the color.
	*/
	void SetColor(const Vector3 &newColor) NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

};