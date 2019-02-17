#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector3.h>

class PointLightComponent final
{

public:

	//Whether or not this point light is enabled.
	bool _Enabled{ false };

	//The color.
	Vector3<float> _Color{ 1.0f, 1.0f, 1.0f };

	//The position.
	Vector3<float> _Position{ Vector3<float>::ZERO };

	//The intensity.
	float _Intensity{ 0.0f };

	//The attenuation distance.
	float _AttenuationDistance{ 25.0f };

};