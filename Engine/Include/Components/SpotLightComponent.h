#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class SpotLightComponent final
{

public:

	//Whether or not this spot light is enabled.
	bool _Enabled{ true };

	//The attenuation distance.
	float _AttenuationDistance{ 25.0f };

	//The intensity.
	float _Intensity{ 1.0f };

	//The inner cutoff angle.
	float _InnerCutoffAngle{ 20.0f };

	//The outer cutoff angle.
	float _OuterCutoffAngle{ 25.0f };

	//The color.
	Vector3 _Color{ 1.0f, 1.0f, 1.0f };

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The position.
	Vector3 _Position{ 0.0f, 0.0f, 0.0f };

};