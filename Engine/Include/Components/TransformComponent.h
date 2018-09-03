#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class TransformComponent final
{

public:

	//The position.
	Vector3 _Position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The scale.
	Vector3 _Scale{ 1.0f, 1.0f, 1.0f };

};