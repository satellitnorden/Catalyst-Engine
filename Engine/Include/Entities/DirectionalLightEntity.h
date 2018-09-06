#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class DirectionalLightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	DirectionalLightEntity() NOEXCEPT;

	/*
	*	Sets the intensity of this directional light entity.
	*/
	void SetIntensity(const float newIntensity) NOEXCEPT;

	/*
	*	Sets the color of this directional light entity.
	*/
	void SetColor(const Vector3 &newColor) NOEXCEPT;

protected:

	/*
	*	Returns the rotation of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

};