#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <LightEntity.h>

class PointLightEntity : public LightEntity
{

public:

	//Universal container of all point light entities.
	static DynamicArray<PointLightEntity *CATALYST_RESTRICT> instances;

	/*
	*	Default constructor.
	*/
	PointLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~PointLightEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns the attenuation distance.
	*/
	float GetAttenuationDistance() const CATALYST_NOEXCEPT { return attenuationDistance; }

	/*
	*	Sets the attenuation distance.
	*/
	void SetAttenuationDistance(const float newAttenuationDistane) CATALYST_NOEXCEPT { attenuationDistance = newAttenuationDistane; }

private:

	//How far away this light travels before fading out completely.
	float attenuationDistance{ 25.0f };

};