#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class SpotLight final
{

public:

	//Whether or not this spot light is enabled.
	bool _Enabled{ false };

	//The color.
	Vector3<float> _Color{ 1.0f, 1.0f, 1.0f };

	//The position.
	Vector3<float> _Position{ Vector3<float>::ZERO };

	//The rotation.
	Vector3<float> _Rotation{ Vector3<float>::ZERO };

	//The intensity.
	float _Intensity{ 0.0f };

	//The attenuation distance.
	float _AttenuationDistance{ 25.0f };

	//The inner cutoff angle.
	float _InnerCutoffAngle{ 20.0f };

	//The outer cutoff angle.
	float _OuterCutoffAngle{ 25.0f };

};