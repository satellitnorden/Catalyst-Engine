#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class TransformComponent final
{

public:

	//The position.
	Vector3<float> _Position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3<float> _Rotation{ 0.0f, 0.0f, 0.0f };

	//The scale.
	Vector3<float> _Scale{ 1.0f, 1.0f, 1.0f };
	
};