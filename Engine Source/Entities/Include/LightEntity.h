#pragma once

//Engine core.
#include <EngineCore.h>

//Entity.
#include <Entity.h>

class LightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	LightEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~LightEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns if this light entity is enabled or not.
	*/
	bool GetEnabled() const CATALYST_NOEXCEPT { return enabled; }

	/*
	*	Sets if this light entity is enabled or not.
	*/
	void SetEnabled(const bool newEnabled) CATALYST_NOEXCEPT { enabled = newEnabled; }

	/*
	*	Returns the light color.
	*/
	const Vector3& GetLightColor() const CATALYST_NOEXCEPT { return lightColor; }

	/*
	*	Sets the light color.
	*/
	void SetLightColor(const Vector3 & newLightcolor) CATALYST_NOEXCEPT { lightColor = newLightcolor; }

	/*
	*	Returns the intensity of the light.
	*/
	float GetIntensity() const CATALYST_NOEXCEPT { return intensity; }

	/*
	*	Sets the intensity of this light.
	*/
	void SetIntensity(const float newIntensity) CATALYST_NOEXCEPT { intensity = newIntensity; }
	
private:

	//Denotes of this light entity is enabled.
	bool enabled{ true };

	//The light color.
	Vector3 lightColor{ 1.0f, 1.0f, 1.0f };

	//The intensity of this light.
	float intensity{ 1.0f };

};