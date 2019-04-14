#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class LightComponent final
{

public:

	//The color.
	Vector3<float> _Color;

	//The position.
	Vector3<float> _Position;

	//The attenuation distance.
	float _AttenuationDistance;

	//The size.
	float _Size;

};