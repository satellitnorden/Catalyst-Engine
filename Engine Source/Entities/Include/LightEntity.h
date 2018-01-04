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
	~LightEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns if this light entity is enabled or not.
	*/
	CATALYST_FORCE_INLINE bool GetEnabled() const CATALYST_NOEXCEPT { return enabled; }

	/*
	*	Sets if this light entity is enabled or not.
	*/
	CATALYST_FORCE_INLINE void SetEnabled(const bool newEnabled) CATALYST_NOEXCEPT { enabled = newEnabled; }

	/*
	*	Returns the light color.
	*/
	CATALYST_FORCE_INLINE const Vector3& GetLightColor() const CATALYST_NOEXCEPT { return lightColor; }

	/*
	*	Sets the light color.
	*/
	CATALYST_FORCE_INLINE void SetLightColor(const Vector3 & newLightcolor) CATALYST_NOEXCEPT { lightColor = newLightcolor; }

	/*
	*	Returns the intensity of the light.
	*/
	CATALYST_FORCE_INLINE float GetIntensity() const CATALYST_NOEXCEPT { return intensity; }

	/*
	*	Sets the intensity of this light.
	*/
	CATALYST_FORCE_INLINE void SetIntensity(const float newIntensity) CATALYST_NOEXCEPT { intensity = newIntensity; }
	
private:

	//Denotes of this light entity is enabled.
	bool enabled{ true };

	//The light color.
	Vector3 lightColor{ 1.0f, 1.0f, 1.0f };

	//The intensity of this light.
	float intensity{ 1.0f };

};