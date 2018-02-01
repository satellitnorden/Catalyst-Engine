#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

class SpotLightComponent
{

public:

	//Whether or not this spot light is enabled.
	bool enabled{ true };

	//The attenuation distance.
	float attenuationDistance{ 25.0f };

	//The intensity.
	float intensity{ 1.0f };

	//The inner cutoff angle.
	float innerCutoffAngle{ 20.0f };

	//The outer cutoff angle.
	float outerCutoffAngle{ 25.0f };

	//The color.
	Vector3 color;

	//The rotation.
	Vector3 rotation;

	//The position.
	Vector3 position;

};