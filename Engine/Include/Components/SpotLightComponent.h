#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class SpotLightComponent final
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
	Vector3 color{ 1.0f, 1.0f, 1.0f };

	//The rotation.
	Vector3 rotation{ 0.0f, 0.0f, 0.0f };

	//The position.
	Vector3 position{ 0.0f, 0.0f, 0.0f };

};