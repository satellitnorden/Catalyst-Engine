#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class PointLightComponent final
{

public:

	//The color.
	Vector3<float> _Color{ 1.0f, 1.0f, 1.0f };

	//The world position.
	Vector3<float> _WorldPosition{ VectorConstants::ZERO };

	//The attenuation distance.
	float _AttenuationDistance{ 25.0f };

	//The intensity.
	float _Intensity{ 0.0f };

};