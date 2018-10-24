#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class DirectionalLight final
{

public:

	//The color.
	Vector3 _Color{ 1.0f, 1.0f, 1.0f };

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The intensity.
	float _Intensity{ 1.0f };

};