#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector3.h>

class SpotLightComponent final
{

public:

	//The color.
	Vector3<float> _Color{ 1.0f, 1.0f, 1.0f };

	//The position.
	Vector3<float> _Position{ Vector3<float>::ZERO };

	//The rotation.
	Vector3<float> _Rotation{ Vector3<float>::ZERO };

	//The attenuation distance.
	float _AttenuationDistance{ 25.0f };

	//Whether or not this spot light is enabled.
	bool _Enabled{ true };

	//The inner cutoff angle.
	float _InnerCutoffAngle{ 20.0f };

	//The intensity.
	float _Intensity{ 1.0f };

	//The outer cutoff angle.
	float _OuterCutoffAngle{ 25.0f };

};