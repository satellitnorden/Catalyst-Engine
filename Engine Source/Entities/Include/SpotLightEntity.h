#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <LightEntity.h>

class SpotLightEntity : public LightEntity
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
	*	Returns the direction of this spot light.
	*/
	Vector3 GetDirection() const CATALYST_NOEXCEPT;

	/*
	*	Returns the attenuation distance.
	*/
	float GetAttenuationDistance() const CATALYST_NOEXCEPT { return attenuationDistance; }

	/*
	*	Sets the attenuation distance.
	*/
	void SetAttenuationDistance(const float newAttenuationDistane) CATALYST_NOEXCEPT { attenuationDistance = newAttenuationDistane; }

	/*
	*	Returns the inner cutoff angle.
	*/
	float GetInnerCutoffAngle() const CATALYST_NOEXCEPT { return innerCutoffAngle; }

	/*
	*	Sets the inner cutoff angle.
	*/
	void SetInnerCutoffAngle(const float newInnerCutoffAngle) CATALYST_NOEXCEPT { innerCutoffAngle = newInnerCutoffAngle; }

	/*
	*	Returns the outer cutoff angle.
	*/
	float GetOuterCutoffAngle() const CATALYST_NOEXCEPT { return outerCutoffAngle; }

	/*
	*	Sets the outer cutoff angle.
	*/
	void SetOuterCutoffAngle(const float newOuterCutoffAngle) CATALYST_NOEXCEPT { outerCutoffAngle = newOuterCutoffAngle; }

private:

	//How far away this light travels before fading out completely.
	float attenuationDistance{ 25.0f };

	//Denotes the inner angle at which this spot light is cut off.
	float innerCutoffAngle{ 20.0f };

	//Denotes the outer angle at which this spot light is cut off.
	float outerCutoffAngle{ 25.0f };

};