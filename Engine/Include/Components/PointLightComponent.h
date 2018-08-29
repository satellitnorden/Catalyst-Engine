#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Math.
#include <Math/Vector3.h>

class PointLightComponent final
{

public:

	//Whether or not this point light is enabled.
	bool enabled{ true };

	//The attenuation distance.
	float attenuationDistance{ 25.0f };

	//The intensity.
	float intensity{ 1.0f };

	//The color.
	Vector3 color{ 1.0f, 1.0f, 1.0f };

	//The position.
	Vector3 position{ 0.0f, 0.0f, 0.0f };

};